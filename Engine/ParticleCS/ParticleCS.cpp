#include "ParticleCS.h"
#include"Log/Log.h"
#include"DXC/DXCManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"functions/function.h"

#include<cassert>



ParticleCS::ParticleCS()
{
	DXF_ = DirectXFunc::GetInstance();
#pragma region RootSignatureを生成する

	//RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma region RootParameter 
	//RootParameter作成。PixelShaderのMAterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[2] = {};

#pragma region Particle
	D3D12_DESCRIPTOR_RANGE descriptorRangeForParticle[1] = {};
	descriptorRangeForParticle[0].BaseShaderRegister = 0;
	descriptorRangeForParticle[0].NumDescriptors = 1;
	descriptorRangeForParticle[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForParticle[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//VSのDescriptorTable
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRangeForParticle;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForParticle);
#pragma endregion
	//PerView
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//すべてで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						//レジスタ番号０とバインド

#pragma endregion

	descriptionRootSignature.pParameters = rootParameters;					//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		//配列の長さ


	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成

	hr = DXF_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region ShaderをCompileする
	DXCManager* DXC = DXCManager::GetInstance();
	//Shaderをコンパイルする
	IDxcBlob* computeShaderBlob = CompileShader(csPass, L"cs_6_0", DXC->GetDxcUtils(), DXC->GetDxcCompiler(), DXC->GetIncludeHandler());
	assert(computeShaderBlob != nullptr);
#pragma endregion

#pragma region PSOを生成
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc{};
	computePipelineDesc.CS = {
		.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
		.BytecodeLength = computeShaderBlob->GetBufferSize()
	};
	computePipelineDesc.pRootSignature = rootSignature_;

	hr = DXF_->GetDevice()->CreateComputePipelineState(&computePipelineDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
#pragma endregion

	Log("Complete Particle Compute Shader Initialize!\n");
}

ParticleCS::~ParticleCS()
{
	rootSignature_->Release();
	graphicsPipelineState_->Release();

	particleResource_->Release();
	perResource_->Release();
}

void ParticleCS::Initialize()
{
	particleResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(Particle) * maxDataCount_);
	particleResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleData_));
	
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
	instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc.Buffer.FirstElement = 0;
	instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc.Buffer.NumElements = (UINT)maxDataCount_;
	instancingDesc.Buffer.StructureByteStride = sizeof(Particle);

	particleHandle = SRVManager::CreateSRV(particleResource_, instancingDesc).gpu;

	perResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(PerView));
	perResource_->Map(0, nullptr, reinterpret_cast<void**>(&perData_));
	perData_->billboardMatrix = MakeIdentity4x4();
	perData_->viewProjection = MakeIdentity4x4();
}

void ParticleCS::PreDraw()
{
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	cmd->SetComputeRootSignature(rootSignature_);
	cmd->SetPipelineState(graphicsPipelineState_);

	cmd->IASetVertexBuffers(1, 0, &vbv_);
	cmd->SetComputeRootDescriptorTable(0, particleHandle);
	cmd->SetComputeRootConstantBufferView(1, perResource_->GetGPUVirtualAddress());

	cmd->Dispatch(1024, 1, 1);
}


