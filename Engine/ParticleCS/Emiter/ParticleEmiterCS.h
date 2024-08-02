#pragma once
#include"Vector3.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleCS/Particle/ParticleData.h"
#include<cstdint>



class ParticleEmiterCS {
public:

	ParticleEmiterCS();
	~ParticleEmiterCS();

	void Initialize();


	void PreDraw(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE chandle);

private:

	DirectXFunc* DXF_ = nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/ParticleEmiter.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//パーティクル最大数
	size_t maxDataCount_ = 1024;

	ID3D12Resource* emiterResource_;
	EmiterSphere* emiterData_ = nullptr;


	ID3D12Resource* perFrameResource_;
	PerFrame* perFrameData_ = nullptr;
};