#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"Particle/ParticleData.h"
#include"struct.h"

class ParticleInitializeCS {
public:
	ParticleInitializeCS();
	~ParticleInitializeCS();

	void Initialize();

	D3D12_GPU_DESCRIPTOR_HANDLE PreDraw();

	void PostDraw();

private:

	DirectXFunc* DXF_ = nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/InitializeParticle.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	size_t maxDataNum_ = 1024;
	ID3D12Resource* particleResource_=nullptr;
	Handles particleSRVHandle_;
	Handles particleHandle_;
};