#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

//スキニングのCS処理
class SkinningCS {

public://パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SkinningCS();

	/// <summary>
	/// デストラクタ
	/// </summary>
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

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PostDraw();

private://**プライベート変数**//

	//DXFのポインタ
	DirectXFunc* DXF_=nullptr;
	
	//モデルとanimationデータ
	const ModelAllData* modelData_=nullptr;

	//リソースまとめ
	struct SRVMaterialResource
	{
		ID3D12Resource* resource;
		Handles handle;
	};

	//CSファイルへのパス
	std::wstring csPass = L"resources/shaders/ComputeShader/Skinning.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//頂点リソース
	SRVMaterialResource vertexResource_;
	VertexData* vertexData_ = nullptr;

	//出力頂点リソース
	SRVMaterialResource outputVerticesResource_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	//スキニング情報まとめ
	struct SkinningInformation
	{
		int32_t numVertices;
	};

	//スキニング情報リソース
	ID3D12Resource* skinningInfoResource_ = nullptr;
	SkinningInformation* skinningInfoData_ = nullptr;
};
