#pragma once
#include"PostEffect/IPipelineStateObject.h"

class PEBloom : public IPipelineStateObject {

public:

	PEBloom();

	void PreDraw()override;

	void Debug()override;


	void Release()override;
private:

	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/Bloom.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	//通常テクスチャ
	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	ID3D12Resource* materialResource_;

	struct PEMaterialData {
		float value;
	};

public:

	PEMaterialData* materialData_;
};