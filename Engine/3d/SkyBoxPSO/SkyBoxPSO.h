#pragma once
#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<vector>

#include"struct.h"
#include"DirectXFunc/DirectXFunc.h"


class SkyBoxPSO {

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	SkyBoxPSO();
	~SkyBoxPSO();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw(FillMode fillmode = FillMode::kSolid, BlendMode blendMode = BlendMode::kNormal);


private:


private:

	DirectXFunc* DXF_;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ =  nullptr ;

	bool isInitialize_ = false;
};
