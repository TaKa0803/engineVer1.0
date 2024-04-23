#pragma once

//#include<dxcapi.h>
#include<d3d12.h>
#include"struct.h"

#include"DirectXFunc/DirectXFunc.h"

class SpritePSO {

public:

	SpritePSO();
	~SpritePSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw(BlendMode blendMode);


private:

	DirectXFunc* DXF_;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_=nullptr;

	//パイプライン
	ID3D12PipelineState*psoState_[int(BlendMode::kCountOfBlendMode)] = { nullptr };
};