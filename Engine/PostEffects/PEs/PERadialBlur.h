#pragma once
#include"PostEffects/IPipelineStateObject.h"
#include"Vector2.h"

class PERadialBlur : public IPipelineStateObject {

public:

	void Initialize() override;

	void PreDraw()override;

	void Debug()override;


	void Release()override;
private:



	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/RadialBlur.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	ID3D12Resource* materialResource_;

	struct PEMaterialData {
		Vector2 center;
		int32_t numSample;
		float blurWidth;
		float value;
	};

public:

	PEMaterialData* materialData_;
};