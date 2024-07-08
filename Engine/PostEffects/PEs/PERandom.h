#pragma once
#include"PostEffects/IPipelineStateObject.h"

class PERandom : public IPipelineStateObject {

public:

	void Initialize() override;

	void PreDraw()override;

	void Debug()override;


	void Release()override;
private:



	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/Random.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	ID3D12Resource* materialResource_;

	struct PEMaterialData {	
		Vector4 color1;
		Vector4 color2;
		float value;
		float time;
		float discardNum;
	};

public:

	PEMaterialData* materialData_;
};