#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleData/ParticleData.h"
#include"struct.h"

class ParticleInitializeCS {
public:
	ParticleInitializeCS();
	~ParticleInitializeCS();

	void Initialize(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE chandle, D3D12_GPU_DESCRIPTOR_HANDLE listhandle);


private:

	Engine::DirectXFunc* DXF_ = nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/InitializeParticle.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	size_t maxDataNum_ = 1024;


};