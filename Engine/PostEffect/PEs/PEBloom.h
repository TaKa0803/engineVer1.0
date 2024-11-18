#pragma once
#include"PostEffect/IPipelineStateObject.h"

class PEBloom : public IPipelineStateObject {

public:

	void Initialize() override;

	void PreDraw()override;

	void Debug()override;


	void Release()override;
private:


	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	//std::wstring psPath = L"resources/shaders/PostEffect/Bloom.PS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/PickHighLuminance.PS.hlsl";

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