#pragma once

#include<d3d12.h>
#include"struct.h"

#include"DirectXFunc/DirectXFunc.h"



class VignettingPSO {

public:

	VignettingPSO();
	~VignettingPSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

private:

	DirectXFunc* DXF_=nullptr;


	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//パイプライン
	ID3D12PipelineState* psoState_ =  nullptr ;
};