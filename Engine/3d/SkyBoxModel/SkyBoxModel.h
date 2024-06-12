#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"WorldTransform/WorldTransform.h"
#include"Camera/Camera.h"
#include"SkyBoxPSO/SkyBoxPSO.h"

struct SkyBoxWorldTransformation {
	Matrix4x4 WVP;
};

struct SkyBoxMaterial
{
	Vector4 color;
};

class SkyBoxModel {

public:

	SkyBoxModel(const std::string& texPath);
	~SkyBoxModel();

	void Initialize();

	void Update();

	void Draw();

public:
	//移動用
	EulerWorldTransform world_;


private:

	DirectXFunc* DXF_=nullptr;
	
	SkyBoxPSO* pso_=nullptr;

	//画像
	D3D12_GPU_DESCRIPTOR_HANDLE texture_;


	ID3D12Resource* vertexResource_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	ID3D12Resource* wvpResource_;
	SkyBoxWorldTransformation* wvpData_ = nullptr;

	//マテリアル
	ID3D12Resource* materialResource_;
	SkyBoxMaterial* materialData_ = nullptr;

	Vector3 uvpos{};
	Vector3 uvscale{ 1.0f,1.0f,1.0f };
	Vector3 uvrotate{};
};