#pragma once
#include"PostEffects/IPipelineStateObject.h"
#include"struct.h"

class PEDepthBasedOutline : public IPipelineStateObject {

public:

	void Initialize() override;

	void PreDraw()override;

	void Debug()override;


	void Release()override;
private:



	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/DepthBasedOutline.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	ID3D12Resource* materialResource_;

	Handles handle;

	struct PEMaterialData {
		Matrix4x4 projectionInverse;
		float value;
		int32_t enableColor;
		
	};

public:

	PEMaterialData* materialData_;
};