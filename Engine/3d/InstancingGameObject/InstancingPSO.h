#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"


//インスタンシング用のPSO
class InstancingPSO {

public://**パブリック関数**//
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	InstancingPSO();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~InstancingPSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw(const FillMode& fillMode, const BlendMode& blendMode);

private://**プライベート変数**//

	//DXFのポインタ
	DirectXFunc* DXF_;

	//vs,psのフルパス
	std::wstring vsPass = L"resources/shaders/Models/InstancingObject.VS.hlsl";
	std::wstring psPass = L"resources/shaders/Models/InstancingObject.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };
};
