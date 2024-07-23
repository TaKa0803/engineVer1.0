#pragma once
#include"PostEffect/IPipelineStateObject.h"

class PEDissolve : public IPipelineStateObject {

public:

	void Initialize() override;

	void PreDraw()override;

	void Debug()override;


	void Release()override;

	void SetDissolveTexture(const std::string& path);
private:

	bool isSetTexture_ = false;
	//指定
	D3D12_GPU_DESCRIPTOR_HANDLE dissolveTexture_;

	D3D12_GPU_DESCRIPTOR_HANDLE noice1_;
	D3D12_GPU_DESCRIPTOR_HANDLE noice2_;


	//パス
	std::wstring vsPath = L"resources/shaders/PostEffect/CopyImage.VS.hlsl";
	std::wstring psPath = L"resources/shaders/PostEffect/Dissolve.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;

	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

	ID3D12Resource* materialResource_;

	

public:

	struct PEMaterialData {
		Vector4 discardColor;
		Vector4 edgeColor;
		float edgeValue;
		float value;
	};


	PEMaterialData* materialData_;
};