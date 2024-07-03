#pragma once
#include"PostEffects/IPipelineStateObject.h"

class PEVignetting : public IPipelineStateObject {

public:

	void Initialize() override;

	void PreDraw()override;

	void Debug()override;
private:



	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/Vignetting.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	ID3D12Resource* materialResource_;

	struct PEMaterialData {
		float value;
	};

public:

	PEMaterialData* materialData_;
};