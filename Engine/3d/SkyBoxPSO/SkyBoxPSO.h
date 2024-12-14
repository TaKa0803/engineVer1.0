#pragma once
#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<vector>

#include"struct.h"
#include"DirectXFunc/DirectXFunc.h"

//スカイボックスのパイプライン
class SkyBoxPSO {

public://**パブリック関数**//

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SkyBoxPSO();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SkyBoxPSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw();

private://**プライベート変数**//

	//DirectXFuncのポインタ
	DirectXFunc* DXF_;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ =  nullptr ;
};
