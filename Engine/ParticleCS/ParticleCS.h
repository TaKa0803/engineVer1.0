#pragma once
#include"Vector4.h"
#include"Matrix.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleCS/Particle/ParticleData.h"


class ParticleCS {
public:
	ParticleCS();
	~ParticleCS();

	void Initialize();

	void PreDraw(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	void PostDraw();
private:

	DirectXFunc* DXF_ = nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/Particle.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//パーティクル最大数
	size_t maxDataCount_ = 1024;

	//シェーダデータ(Particle
	ID3D12Resource* particleResource_=nullptr;

	//Perリソース
	ID3D12Resource* perResource_=nullptr;
	PerView* perData_ = nullptr;
	
};