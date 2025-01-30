#include "SkinningCS.h"
#include"functions/function.h"
#include"Log/Log.h"
#include"DXC/DXCManager.h"
#include"SRVManager/SRVManager.h"
#include"UAVManager/UAVManager.h"

#include<cassert>

using namespace Engine;

SkinningCS::SkinningCS()
{
	//DXFのインスタンス取得
	DXF_ = Engine::DirectXFunc::GetInstance();
#pragma region RootSignatureを生成する
	//RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma region RootParameter 
	//RootParameter作成。PixelShaderのMAterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

#pragma region palette
	D3D12_DESCRIPTOR_RANGE descriptorRangePalette[1] = {};
	descriptorRangePalette[0].BaseShaderRegister = 0;								//0から始まる
	descriptorRangePalette[0].NumDescriptors = 1;									//数
	descriptorRangePalette[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;			//SRVを使う
	descriptorRangePalette[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//DescriptorHeapを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRangePalette;				//tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangePalette);	//tableで利用する
#pragma endregion

#pragma region inputIndex
	D3D12_DESCRIPTOR_RANGE descriptorRangeInputIndex[1] = {};
	descriptorRangeInputIndex[0].BaseShaderRegister = 1;								//0から始まる
	descriptorRangeInputIndex[0].NumDescriptors = 1;									//数
	descriptorRangeInputIndex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;			//SRVを使う
	descriptorRangeInputIndex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//DescriptorHeapを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeInputIndex;				//tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeInputIndex);	//tableで利用する
#pragma endregion

#pragma region influence
	D3D12_DESCRIPTOR_RANGE descriptorRangeInfluence[1] = {};
	descriptorRangeInfluence[0].BaseShaderRegister = 2;								//0から始まる
	descriptorRangeInfluence[0].NumDescriptors = 1;									//数
	descriptorRangeInfluence[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;			//SRVを使う
	descriptorRangeInfluence[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//DescriptorHeapを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeInfluence;				//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeInfluence);	//tableで利用する
#pragma endregion

#pragma region outputVertex
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutputVertex[1] = {};
	descriptorRangeOutputVertex[0].BaseShaderRegister = 0;								//0から始まる
	descriptorRangeOutputVertex[0].NumDescriptors = 1;									//数
	descriptorRangeOutputVertex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;			//UAVを使う
	descriptorRangeOutputVertex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;;		//DescriptorHeapを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRangeOutputVertex;				//tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeOutputVertex);	//tableで利用する
#pragma endregion

#pragma region skinningInformation

	//PSのDescriptorTable
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;			//CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 0;							//レジスタ番号０とバインド

#pragma endregion

	descriptionRootSignature.pParameters = rootParameters;					//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		//配列の長さ

#pragma region Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイニアリング
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
#pragma endregion

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//エラーチェック
	if (FAILED(hr)) {
		//ログ出力
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成
	hr = DXF_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

#pragma endregion
#pragma region ShaderをCompileする
	//インスタンス取得
	DXCManager* DXC = DXCManager::GetInstance();
	//Shaderをコンパイルする
	IDxcBlob* computeShaderBlob = CompileShader(csPass, L"cs_6_0", DXC->GetDxcUtils(), DXC->GetDxcCompiler(), DXC->GetIncludeHandler());
	assert(computeShaderBlob != nullptr);
#pragma endregion

#pragma region PSOを生成
	//生成前にパイプラインデスク設定
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc{};
	computePipelineDesc.CS = {
		.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
		.BytecodeLength = computeShaderBlob->GetBufferSize()
	};
	//RootSingnature設定
	computePipelineDesc.pRootSignature = rootSignature_;
	//実際に生成
	hr = DXF_->GetDevice()->CreateComputePipelineState(&computePipelineDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
#pragma endregion

	//ログに出力
	Log("Complete Skinning Compute Shader Initialize!\n");
}

SkinningCS::~SkinningCS()
{
	//各リソースの解放
	rootSignature_->Release();
	rootSignature_ = nullptr;

	graphicsPipelineState_->Release();
	graphicsPipelineState_ = nullptr;

	vertexResource_.resource->Release();

	outputVerticesResource_.resource->Release();
	skinningInfoResource_->Release();
}


void SkinningCS::Initialize(const ModelAllData& data)
{
	//ポインタに情報設定
	modelData_ = &data;

	//兆点サイズ取得
	size_t verticesSize = modelData_->model.vertices.size();

#pragma region 各シェーダデータ
#pragma region Vertex
	//使っていないSRVのハンドル取得
	Handles vhandles = SRVManager::GetInstance()->CreateNewSRVHandles();
	//リソース生成
	vertexResource_.resource = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * verticesSize);
	vertexResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, data.model.vertices.data(), sizeof(VertexData) * data.model.vertices.size());

	vertexResource_.handle = vhandles;

	D3D12_SHADER_RESOURCE_VIEW_DESC vSRVDesc{};
	vSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	vSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	vSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	vSRVDesc.Buffer.FirstElement = 0;
	vSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	vSRVDesc.Buffer.NumElements = UINT(verticesSize);
	vSRVDesc.Buffer.StructureByteStride = sizeof(VertexData);
	//SRV生成
	DXF_->GetDevice()->CreateShaderResourceView(vertexResource_.resource, &vSRVDesc, vertexResource_.handle.cpu);
#pragma endregion
#pragma region OutputVertices
	//UAV生成
	outputVerticesResource_.resource = DXF_->CreateUAVBufferResource(sizeof(VertexData) * verticesSize);

	
	vbv_.BufferLocation = outputVerticesResource_.resource->GetGPUVirtualAddress();
	vbv_.SizeInBytes = UINT(sizeof(VertexData) * verticesSize);
	vbv_.StrideInBytes = sizeof(VertexData);

	Handles ohandles = SRVManager::GetInstance()->CreateNewSRVHandles();
	outputVerticesResource_.handle = ohandles;

	D3D12_UNORDERED_ACCESS_VIEW_DESC oUAVDesc{};
	oUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	oUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	oUAVDesc.Buffer.FirstElement = 0;
	oUAVDesc.Buffer.NumElements = UINT(verticesSize);
	oUAVDesc.Buffer.CounterOffsetInBytes = 0;
	oUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	oUAVDesc.Buffer.StructureByteStride = sizeof(VertexData);
	//UAV作成
	DXF_->GetDevice()->CreateUnorderedAccessView(outputVerticesResource_.resource, nullptr, &oUAVDesc, outputVerticesResource_.handle.cpu);
#pragma endregion
#pragma region SkinningInfomation
	//リソース生成
	skinningInfoResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(SkinningInformation));
	skinningInfoResource_->Map(0, nullptr, reinterpret_cast<void**>(&skinningInfoData_));
	skinningInfoData_->numVertices = (int32_t)verticesSize;
#pragma endregion
#pragma endregion
}

D3D12_VERTEX_BUFFER_VIEW SkinningCS::PreDraw()
{
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();
	//シグネチャ設定
	cmd->SetComputeRootSignature(rootSignature_);
	//パイプライン設定
	cmd->SetPipelineState(graphicsPipelineState_);

	//テーブル設定
	cmd->SetComputeRootDescriptorTable(0, modelData_->skinCluster.paletteSrvHandle.second);
	cmd->SetComputeRootDescriptorTable(1, vertexResource_.handle.gpu);
	cmd->SetComputeRootDescriptorTable(2, modelData_->skinCluster.influenceSrvHandle.second);
	cmd->SetComputeRootDescriptorTable(3, outputVerticesResource_.handle.gpu);
	cmd->SetComputeRootConstantBufferView(4, skinningInfoResource_->GetGPUVirtualAddress());

	//CS処理
	cmd->Dispatch(UINT(modelData_->model.vertices.size() + 1023) / 1024, 1, 1);

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = outputVerticesResource_.resource;  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);

	return vbv_;
}

void SkinningCS::PostDraw()
{
	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = outputVerticesResource_.resource;  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();
	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);
}
