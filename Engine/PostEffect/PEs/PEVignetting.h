#pragma once
#include"PostEffect/IPipelineStateObject.h"

class PEVignetting : public IPipelineStateObject {

public:

	PEVignetting();

	void PreDraw()override;

	void Debug()override;


	void Release()override;
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
		float darkness;
		float effective;
	};

public:

	static PEMaterialData* materialData_;
};