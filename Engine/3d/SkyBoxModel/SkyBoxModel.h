#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"WorldTransform/WorldTransform.h"
#include"Camera/Camera.h"
#include"SkyBoxPSO/SkyBoxPSO.h"



//スカイボックスモデル
class SkyBoxModel {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="texPath">画像パス</param>
	SkyBoxModel(const std::string& texPath);
	~SkyBoxModel();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public://**パブリック変数**//

	//移動用
	EulerWorldTransform world_;

private://**プライベート変数**//

	//
	struct SkyBoxWorldTransformation {
		Matrix4x4 WVP;
	};

	struct SkyBoxMaterial
	{
		Vector4 color;
	};

	//DXFのポインタ
	Engine::DirectXFunc* DXF_=nullptr;
	
	//PSOのポインタ
	SkyBoxPSO* pso_=nullptr;

	//画像
	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	//頂点リソース
	ID3D12Resource* vertexResource_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点リソース
	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	//WVPのリソース
	ID3D12Resource* wvpResource_;
	SkyBoxWorldTransformation* wvpData_ = nullptr;

	//マテリアル
	ID3D12Resource* materialResource_;
	SkyBoxMaterial* materialData_ = nullptr;

	//UVのパラメータ
	//座標
	Vector3 uvpos{};
	//サイズ
	Vector3 uvscale{ 1.0f,1.0f,1.0f };
	//回転量
	Vector3 uvrotate{};
};