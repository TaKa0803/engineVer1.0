#pragma once
#include"PostEffect/IPipelineStateObject.h"

class PEHSVFilter : public IPipelineStateObject {

public:

	PEHSVFilter();

	void PreDraw()override;

	void Debug()override;


	void Release()override;
private:

	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/HSVFilter.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	ID3D12Resource* materialResource_;

	struct PEMaterialData {
		float hue;
		float saturation;
		float value;
		float effective;
	};

public:

	static PEMaterialData* materialData_;
};