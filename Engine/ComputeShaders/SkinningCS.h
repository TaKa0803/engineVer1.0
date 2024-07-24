#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

class SkinningCS {

public:

	SkinningCS();
	~SkinningCS();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const ModelAllData& data);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	D3D12_VERTEX_BUFFER_VIEW PreDraw();

private:


private:

	//モデルとanimationデータ
	const ModelAllData* modelData_=nullptr;

	struct SRVMaterialResource
	{
		ID3D12Resource* resource;
		Handles handle;
	};

	DirectXFunc* DXF_=nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/Skinning.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	bool isInitialize_ = false;

	SRVMaterialResource wellResource_;
	std::span<WellForGPU> wellPalette_;

	SRVMaterialResource vertexResource_;
	VertexData* vertexData_ = nullptr;

	SRVMaterialResource influenceResource_;
	std::span<VertexInfluence> influenceData_;

	SRVMaterialResource outputVerticesResource_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	//VertexData* outputVerticesData_ = nullptr;

	struct SkinningInformation
	{
		int32_t numVertices;
	};

	ID3D12Resource* skinningInfoResource_ = nullptr;
	SkinningInformation* skinningInfoData_ = nullptr;
};
