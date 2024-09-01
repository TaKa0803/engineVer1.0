#include "ParticleEmiterCS.h"
#include"Log/Log.h"
#include"DXC/DXCManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"functions/function.h"
#include"UAVManager/UAVManager.h"

#include<cassert>

ParticleEmiterCS::ParticleEmiterCS()
{
	DXF_ = DirectXFunc::GetInstance();
#pragma region RootSignatureを生成する

	//RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma region RootParameter 
	//RootParameter作成。PixelShaderのMAterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

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

	//エミッタデータ
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//すべてで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						//レジスタ番号０とバインド

	//PerFrame
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//すべてで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;						//レジスタ番号０とバインド


	//Particle
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 1;								//0から始まる
	descriptorRange[0].NumDescriptors = 1;									//数
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;			//UAVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;;		//DescriptorHeapを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;				//tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//tableで利用する


	//list
	D3D12_DESCRIPTOR_RANGE descriptorRangeList[1] = {};
	descriptorRangeList[0].BaseShaderRegister = 2;								//0から始まる
	descriptorRangeList[0].NumDescriptors = 1;									//数
	descriptorRangeList[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;			//UAVを使う
	descriptorRangeList[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;;		//DescriptorHeapを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRangeList;				//tableの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeList);	//tableで利用する

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




	perFrameResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(PerFrame));
	perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&perFrameData_));
	perFrameData_->time = 0;
	perFrameData_->deltaTime = 1.0f / 60.0f;
}

ParticleEmiterCS::~ParticleEmiterCS()
{
	rootSignature_->Release();
	graphicsPipelineState_->Release();
	perFrameResource_->Release();
}

void ParticleEmiterCS::Initialize(D3D12_GPU_VIRTUAL_ADDRESS emiterDataAddress,EmiterSphere* emiterData)
{
	emiterDataAddress_ = emiterDataAddress;
	//ポインタを共用
	emiterData_ = emiterData;
}

void ParticleEmiterCS::Update(bool onlyImpact)
{

	perFrameData_->time++;


	if (!onlyImpact) {
		//仮でデルタタイム直
		float deltaTime = 1.0f / 60.0f;

		//エミッター更新
		emiterData_->frequencyTime += deltaTime;
		
		//差出間隔を上回ったら射出許可を出して時間を調整
		if (emiterData_->frequency <= emiterData_->frequencyTime) {
			emiterData_->frequencyTime -= emiterData_->frequency;
			emiterData_->emit = 1;
		}
		else {
			emiterData_->emit = 0;
		}
	}
	else {
		emiterData_->emit = 0;
	}

}

void ParticleEmiterCS::EmitGPU(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE chandle, D3D12_GPU_DESCRIPTOR_HANDLE listhandle)
{

	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	ID3D12DescriptorHeap* des[] = { UAVManager::GetInstance()->GetDescriptorHeap() };

	cmd->SetDescriptorHeaps(1, des);
	cmd->SetComputeRootSignature(rootSignature_);
	cmd->SetPipelineState(graphicsPipelineState_);

	cmd->SetComputeRootDescriptorTable(0, handle);
	cmd->SetComputeRootConstantBufferView(1, emiterDataAddress_);
	cmd->SetComputeRootConstantBufferView(2, perFrameResource_->GetGPUVirtualAddress());
	cmd->SetComputeRootDescriptorTable(3, chandle);
	cmd->SetComputeRootDescriptorTable(4, listhandle);

	cmd->Dispatch(1, 1, 1);

}


