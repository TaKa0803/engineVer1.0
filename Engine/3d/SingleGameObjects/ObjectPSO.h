#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<vector>

#include"struct.h"
#include"DirectXFunc/DirectXFunc.h"

//シングルオブジェクトのパイプライン
class ObjectPSO {

public://**パブリック関数**//

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ObjectPSO();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ObjectPSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw(FillMode fillmode,BlendMode blendMode);

private://**プライベート変数**//

	//DXFのポインタ
	Engine::DirectXFunc* DXF_;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };

	bool isInitialize_ = false;
};

