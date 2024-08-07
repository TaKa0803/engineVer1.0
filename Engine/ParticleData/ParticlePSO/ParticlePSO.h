#pragma once
#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"


class ParticlePSO {
public:
	ParticlePSO();
	~ParticlePSO();

	void Initialize();

	void PreDraw(const BlendMode& blendMode=BlendMode::kNormal);

private:
	DirectXFunc* DXF_=nullptr;

	std::wstring vsPass = L"resources/shaders/Particles/Particle.VS.hlsl";
	std::wstring psPass = L"resources/shaders/Particles/Particle.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_[int(BlendMode::kCountOfBlendMode)] = { nullptr };
};