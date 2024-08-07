#pragma once
#include"Vector4.h"
#include"Matrix.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleData/ParticleData.h"


class ParticleUpdateCS {
public:
	ParticleUpdateCS();
	~ParticleUpdateCS();

	void Initialize();

	void PreDraw(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_VIRTUAL_ADDRESS adress, D3D12_GPU_DESCRIPTOR_HANDLE chandle, D3D12_GPU_DESCRIPTOR_HANDLE listhandle);

private:

	DirectXFunc* DXF_ = nullptr;

	std::wstring csPass = L"resources/shaders/ComputeShader/Particle.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//パーティクル最大数
	size_t maxDataCount_ = 1024;


	
};