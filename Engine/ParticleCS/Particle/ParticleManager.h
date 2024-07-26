#pragma once
#include"Vector2.h"
#include"Vector4.h"
#include"Matrix.h"
#include"ParticleData.h"
#include"ParticlePSO/ParticlePSO.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleCS/ParticleCS.h"
#include"ParticleCS/ParticleInitialize.h"

#include<stdint.h>

struct ParticleMaterialData
{
	Vector4 color;
	Matrix4x4 uvTransform;
	int32_t enableTex;
	float discardNum;
};

class ParticleManager {

public:
	ParticleManager();
	~ParticleManager();

	//初期化処理
	void Initialize(int tex);

	//データ更新処理


	//描画
	void Draw();

private:

	DirectXFunc* DXF_;
	
	ModelData modelData_;

	std::unique_ptr<ParticlePSO>pso_;

	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	std::unique_ptr<ParticleInitializeCS> particleInitializeCS_;
	std::unique_ptr<ParticleCS>particleUpdateCS_;

	D3D12_GPU_DESCRIPTOR_HANDLE initializeGPUData_;

#pragma region リソース関係
	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;


	ID3D12Resource* particleResource_;
	Particle* particleData_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE wvpHandles_;

	ID3D12Resource* perResource_;
	PerView* perViewData_ = nullptr;

	ID3D12Resource* materialResource_;
	ParticleMaterialData* materialData_=nullptr;
#pragma endregion



};