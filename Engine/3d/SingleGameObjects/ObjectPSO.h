#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<vector>

#include"struct.h"
#include"DirectXFunc/DirectXFunc.h"


class ObjectPSO {

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	ObjectPSO();
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


private:


private:

	DirectXFunc* DXF_;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };

	bool isInitialize_ = false;
};

