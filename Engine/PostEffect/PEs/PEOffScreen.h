#pragma once
#include"PostEffect/IPipelineStateObject.h"

class PEOffScreen : public IPipelineStateObject {

public:

	PEOffScreen();
	~PEOffScreen();

	
	
	void PreDraw()override;

	void Debug()override;

	void Release()override;
private:



	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/CopyImage.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;


	struct PEMaterialData {
		float value;
	};

public:

};