#pragma once

#include<d3d12.h>
#include"struct.h"

#include"DirectXFunc/DirectXFunc.h"

struct FullScreenData
{
	int32_t type;
	float sepiaValue;
};

class OffScreenRendering {

public:

	OffScreenRendering();
	~OffScreenRendering();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 
	/// </summary>
	void Debug();

	static FullScreenData* materialData_;

private:

	DirectXFunc* DXF_=nullptr;

	ID3D12Resource* materialResource_;
	
	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//パイプライン
	ID3D12PipelineState* psoState_ =  nullptr ;
};