#pragma once

//#include<dxcapi.h>
#include<d3d12.h>
#include<wrl.h>
#include"struct.h"

#include"DirectXFunc/DirectXFunc.h"

//スプライトのパイプライン
class SpritePSO {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpritePSO();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpritePSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw(BlendMode blendMode);

private://**プライベート変数**//

	//DXFのポインタ
	Engine::DirectXFunc* DXF_;

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_=nullptr;

	//パイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState>psoState_[int(BlendMode::kCountOfBlendMode)] = { nullptr };
};