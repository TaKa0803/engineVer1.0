#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

class SkinningCS {

public:

	SkinningCS();
	~SkinningCS();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw(const FillMode& fillMode, const BlendMode& blendMode);

private:


private:

	DirectXFunc* DXF_;

	std::wstring csPass = L"resources/shaders/InstancingObject.VS.hlsl";
	std::wstring psPass = L"resources/shaders/InstancingObject.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ =  nullptr ;

	bool isInitialize_ = false;
};
