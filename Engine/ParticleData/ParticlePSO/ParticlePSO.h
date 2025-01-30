#pragma once
#include<string>
#include<d3d12.h>
#include<dxcapi.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

//パーティクル用のパイプライン
class ParticlePSO {
public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticlePSO();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticlePSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="blendMode"></param>
	void PreDraw(const BlendMode& blendMode=BlendMode::kNormal);

private://**プライベート変数**//

	//DXFのポインタ
	Engine::DirectXFunc* DXF_=nullptr;

	//VS、PSのHLSLまでのパス
	std::wstring vsPass = L"resources/shaders/Particles/Particle.VS.hlsl";
	std::wstring psPass = L"resources/shaders/Particles/Particle.PS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_[int(BlendMode::kCountOfBlendMode)] = { nullptr };
};