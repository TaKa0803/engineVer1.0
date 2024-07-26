#pragma once
#include"Vector2.h"
#include"ParticleData.h"
#include"ParticlePSO/ParticlePSO.h"
#include"DirectXFunc/DirectXFunc.h"

class ParticleManager {

public:
	ParticleManager();
	~ParticleManager();

	//初期化処理
	void Initialize(int tex);

	//データ更新処理
	void Update();

	//描画
	void Draw();

	void SetData(const Particle& data);
private:

	DirectXFunc* DXF_;
	
	ModelData modelData_;

	std::unique_ptr<ParticlePSO>pso_;

	D3D12_GPU_DESCRIPTOR_HANDLE texture_;


	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;


	ID3D12Resource* wvpResource_;
	Particle* particleData_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE wvpHandles_;

	ID3D12Resource* perResource_;
	PerView* perViewData_ = nullptr;

	struct MaterialData
	{
		Vector4 color;
		Matrix4x4 uvTransform;
		int32_t enableTex;
		float discardNum;
	};

	ID3D12Resource* materialResource_;
	MaterialData* materialData_=nullptr;

};