#pragma once
#include"Vector3.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleData/ParticleData.h"
#include<cstdint>



class ParticleEmiterCS {
public:

	//各生成処理
	ParticleEmiterCS();
	//解放処理
	~ParticleEmiterCS();

	//初期化処理
	void Initialize(D3D12_GPU_VIRTUAL_ADDRESS emiterDataAddress,EmiterSphere* emiterData);

	//更新処理
	void Update(bool onlyImpact);

	//CSの処理実行
	void EmitGPU(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE chandle, D3D12_GPU_DESCRIPTOR_HANDLE listhand);

	EmiterSphere* GetEmiterData() { return emiterData_; }
private:

	DirectXFunc* DXF_ = nullptr;
	//CSのパス
	std::wstring csPass = L"resources/shaders/ComputeShader/ParticleEmiter.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//パーティクル最大数
	size_t maxDataCount_ = 1024;

	D3D12_GPU_VIRTUAL_ADDRESS emiterDataAddress_;
	EmiterSphere* emiterData_ = nullptr;

	ID3D12Resource* perFrameResource_;
	PerFrame* perFrameData_ = nullptr;

};