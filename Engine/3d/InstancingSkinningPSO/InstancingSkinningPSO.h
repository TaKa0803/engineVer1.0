#pragma once
#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"



class InstancingSkinningPSO {

public:

	InstancingSkinningPSO();
	~InstancingSkinningPSO();

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

	std::wstring vsPass = L"resources/shaders/InstancingSkinningOBJ3D.VS.hlsl";
	std::wstring psPass = L"resources/shaders/InstancingSkinningOBJ3D.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };



	bool isInitialize_ = false;
};
