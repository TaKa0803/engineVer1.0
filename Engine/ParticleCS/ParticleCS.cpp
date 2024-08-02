#include "ParticleCS.h"
#include"Log/Log.h"
#include"DXC/DXCManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"functions/function.h"
#include"UAVManager/UAVManager.h"

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
	descriptorRangeForParticle[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
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


	Log("Complete ParticleUpdate ComputeShader Initialize!\n");
}

ParticleCS::~ParticleCS()
{
	rootSignature_->Release();
	graphicsPipelineState_->Release();


}

void ParticleCS::Initialize()
{

}

void ParticleCS::PreDraw(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_VIRTUAL_ADDRESS adress)
{
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	ID3D12DescriptorHeap* des[] = { UAVManager::GetInstance()->GetDescriptorHeap() };

	cmd->SetDescriptorHeaps(1, des);
	cmd->SetComputeRootSignature(rootSignature_);
	cmd->SetPipelineState(graphicsPipelineState_);
	
	cmd->SetComputeRootDescriptorTable(0, handle);
	cmd->SetComputeRootConstantBufferView(1, adress);

	cmd->Dispatch(1024, 1, 1);
	
}




