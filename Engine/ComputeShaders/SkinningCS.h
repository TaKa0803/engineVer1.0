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
	void Initialize(const ModelAllData& data, size_t jointsSize, size_t verticesSize);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw();

private:


private:

	//モデルとanimationデータ
	const ModelAllData* modelData_;

	//スケルトンデータ
	Skeleton skeleton_;
	//スキンanimationデータ
	SkinCluster skinCluster_;

	struct SRVMaterialResource
	{
		ID3D12Resource* resource;
		Handles handle;
	};

	DirectXFunc* DXF_;

	std::wstring csPass = L"resources/shaders/InstancingObject.VS.hlsl";

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
	VertexData* outputVerticesData_ = nullptr;

	struct SkinningInformation
	{
		int32_t numVertices;
	};

	ID3D12Resource* skinningInfoResource_ = nullptr;
	SkinningInformation* skinningInfoData_ = nullptr;
};
