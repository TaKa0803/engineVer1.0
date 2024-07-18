#include "SkinningCS.h"
#include"functions/function.h"
#include"Log/Log.h"
#include"DXC/DXCManager.h"
#include"SRVManager/SRVManager.h"

#include<cassert>

SkinningCS::SkinningCS()
{
	if (isInitialize_) {
		return;
	}
	isInitialize_ = true;

	DXF_ = DirectXFunc::GetInstance();
#pragma region RootSignatureを生成する

	//RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma region RootParameter 
	//RootParameter作成。PixelShaderのMAterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[6] = {};

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
	descriptorRangeOutputVertex[0].BaseShaderRegister = 3;								//0から始まる
	descriptorRangeOutputVertex[0].NumDescriptors = 1;									//数
	descriptorRangeOutputVertex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;			//SRVを使う
	descriptorRangeOutputVertex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;;		//DescriptorHeapを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRangeOutputVertex;				//tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeOutputVertex);	//tableで利用する
#pragma endregion

#pragma region skinningInformation
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 4;								//0から始まる
	descriptorRange[0].NumDescriptors = 1;									//数
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;			//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算	

	//PSのDescriptorTable
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//DescriptorHeapを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;					//PixelShaderで使う 
	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRange;				//tableの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//tableで利用する
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
	IDxcBlob* computeShaderBlob = CompileShader(csPass, L"vs_6_0", DXC->GetDxcUtils(), DXC->GetDxcCompiler(), DXC->GetIncludeHandler());
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

	Log("Complete Skinning Compute Shader Initialize!\n");
}

SkinningCS::~SkinningCS()
{
	rootSignature_->Release();
	rootSignature_ = nullptr;

	graphicsPipelineState_->Release();
	graphicsPipelineState_ = nullptr;


}


void SkinningCS::Initialize(const ModelAllData& data,size_t jointsSize,size_t verticesSize)
{
	//ポインタに情報設定
	modelData_ = &data;

#pragma region 各シェーダデータ


#pragma region WellPalette
	WellForGPU* wData = nullptr;
	Handles handles = SRVManager::GetInstance()->CreateNewSRVHandles();
	wellResource_.resource = CreateBufferResource(DXF_->GetDevice(), sizeof(WellForGPU) * jointsSize);
	wellResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&wData));
	wellPalette_ = { wData , jointsSize };
	wellResource_.handle = handles;

	D3D12_SHADER_RESOURCE_VIEW_DESC pSRVDesc{};
	pSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	pSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	pSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	pSRVDesc.Buffer.FirstElement = 0;
	pSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	pSRVDesc.Buffer.NumElements = UINT(jointsSize);
	pSRVDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	DXF_->GetDevice()->CreateShaderResourceView(wellResource_.resource, &pSRVDesc, wellResource_.handle.cpu);
#pragma endregion
#pragma region Vertex
	Handles vhandles = SRVManager::GetInstance()->CreateNewSRVHandles();
	vertexResource_.resource = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * verticesSize);
	vertexResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	vertexResource_.handle = vhandles;

	D3D12_SHADER_RESOURCE_VIEW_DESC vSRVDesc{};
	vSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	vSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	vSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	vSRVDesc.Buffer.FirstElement = 0;
	vSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	vSRVDesc.Buffer.NumElements = UINT(verticesSize);
	vSRVDesc.Buffer.StructureByteStride = sizeof(VertexData);

	DXF_->GetDevice()->CreateShaderResourceView(vertexResource_.resource, &vSRVDesc, vertexResource_.handle.cpu);
#pragma endregion
#pragma region Influence
	VertexInfluence* vertexInfluence = nullptr;
	Handles ihandles = SRVManager::GetInstance()->CreateNewSRVHandles();
	influenceResource_.resource = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexInfluence) * verticesSize);
	influenceResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexInfluence));
	influenceData_ = { vertexInfluence,verticesSize };
	influenceResource_.handle = ihandles;

	D3D12_SHADER_RESOURCE_VIEW_DESC iSRVDesc{};
	iSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	iSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	iSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	iSRVDesc.Buffer.FirstElement = 0;
	iSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	iSRVDesc.Buffer.NumElements = UINT(verticesSize);
	iSRVDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);

	DXF_->GetDevice()->CreateShaderResourceView(influenceResource_.resource, &iSRVDesc, influenceResource_.handle.cpu);
#pragma endregion

#pragma region OutputVertices
	Handles ohandles = SRVManager::GetInstance()->CreateNewSRVHandles();
	outputVerticesResource_.resource = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * verticesSize);
	outputVerticesResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&influenceData_));
	outputVerticesResource_.handle = ohandles;

	D3D12_UNORDERED_ACCESS_VIEW_DESC oSRVDesc{};
	oSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	oSRVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	oSRVDesc.Buffer.FirstElement = verticesSize;
	oSRVDesc.Buffer.CounterOffsetInBytes = 0;
	oSRVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	oSRVDesc.Buffer.NumElements = UINT(verticesSize);
	oSRVDesc.Buffer.StructureByteStride = sizeof(VertexData);

	DXF_->GetDevice()->CreateUnorderedAccessView(outputVerticesResource_.resource,nullptr, &oSRVDesc, outputVerticesResource_.handle.cpu);
#pragma endregion

#pragma region SkinningInfomation

	

	skinningInfoResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(SkinningInformation));
	skinningInfoResource_->Map(0, nullptr, reinterpret_cast<void**>(&skinningInfoData_));
	skinningInfoData_->numVertices = (int32_t)verticesSize;

#pragma endregion

#pragma endregion



}

void SkinningCS::PreDraw()
{
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	cmd->SetComputeRootSignature(rootSignature_);
	cmd->SetPipelineState(graphicsPipelineState_);

	cmd->SetComputeRootDescriptorTable(0, wellResource_.handle.gpu);
	cmd->SetComputeRootDescriptorTable(1, vertexResource_.handle.gpu);
	cmd->SetComputeRootDescriptorTable(2, influenceResource_.handle.gpu);
	cmd->SetComputeRootDescriptorTable(3, outputVerticesResource_.handle.gpu);
	cmd->SetGraphicsRootConstantBufferView(4, skinningInfoResource_->GetGPUVirtualAddress());

	cmd->Dispatch(UINT(modelData_->model.vertices.size() + 1023) / 1024, 1, 1);
}
