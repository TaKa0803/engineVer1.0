#pragma once
#include"Vector4.h"
#include"Matrix.h"
#include"DirectXFunc/DirectXFunc.h"





class ParticleCS {
public:
	ParticleCS();
	~ParticleCS();

	void Initialize();

	void PreDraw();

	void PostDraw();

private:

	DirectXFunc* DXF_ = nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/Particle.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;
};