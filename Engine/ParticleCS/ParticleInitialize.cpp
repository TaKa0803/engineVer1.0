#include "ParticleInitialize.h"
#include"Log/Log.h"
#include"DXC/DXCManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"UAVManager/UAVManager.h"
#include<cassert>


ParticleInitializeCS::ParticleInitializeCS()
{
	DXF_ = DirectXFunc::GetInstance();
#pragma region RootSignatureを生成する

	//RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma region RootParameter 
	//RootParameter作成。PixelShaderのMAterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[1] = {};

	//Particle
	D3D12_DESCRIPTOR_RANGE descriptorRangeParticle[1] = {};
	descriptorRangeParticle[0].BaseShaderRegister = 0;								//0から始まる
	descriptorRangeParticle[0].NumDescriptors = 1;									//数
	descriptorRangeParticle[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;			//UAVを使う
	descriptorRangeParticle[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;;		//DescriptorHeapを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRangeParticle;				//tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeParticle);	//tableで利用する

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

ParticleInitializeCS::~ParticleInitializeCS()
{
	rootSignature_->Release();
	graphicsPipelineState_->Release();
	particleResource_->Release();
}

void ParticleInitializeCS::Initialize()
{
	particleResource_ = CreateUAVBufferResource(DXF_->GetDevice(), sizeof(Particle) * maxDataNum_);

#pragma region UAVのHandle生成
	particleHandle_ = UAVManager::GetInstance()->GetDescriptorHandle();

	D3D12_UNORDERED_ACCESS_VIEW_DESC oUAVDesc{};
	oUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	oUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	oUAVDesc.Buffer.FirstElement = 0;
	oUAVDesc.Buffer.NumElements = UINT(maxDataNum_);
	oUAVDesc.Buffer.CounterOffsetInBytes = 0;
	oUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	oUAVDesc.Buffer.StructureByteStride = sizeof(Particle);

	DXF_->GetDevice()->CreateUnorderedAccessView(particleResource_, nullptr, &oUAVDesc, particleHandle_.cpu);

#pragma endregion


#pragma region SRVのHandle生成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
	instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc.Buffer.FirstElement = 0;
	instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc.Buffer.NumElements = UINT(maxDataNum_);
	instancingDesc.Buffer.StructureByteStride = sizeof(Particle);

	particleSRVHandle_ = SRVManager::CreateSRV(particleResource_, instancingDesc);
#pragma endregion


	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	ID3D12DescriptorHeap* des[] = { UAVManager::GetInstance()->GetDescriptorHeap() };

	cmd->SetDescriptorHeaps(1,des);
	cmd->SetComputeRootSignature(rootSignature_);
	cmd->SetPipelineState(graphicsPipelineState_);


	cmd->SetComputeRootDescriptorTable(0, particleHandle_.gpu);
	cmd->Dispatch(1024, 1, 1);
}

D3D12_GPU_DESCRIPTOR_HANDLE ParticleInitializeCS::PreDraw()
{
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = particleResource_;  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);

	return particleSRVHandle_.gpu;
}

void ParticleInitializeCS::PostDraw()
{
	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = particleResource_;  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();
	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);
}

