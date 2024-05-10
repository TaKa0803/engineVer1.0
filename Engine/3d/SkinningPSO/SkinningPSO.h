#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<vector>

#include"struct.h"
#include"DirectXFunc/DirectXFunc.h"

class SkinningPSO
{
public:

	SkinningPSO();
	~SkinningPSO();

	void Initialize();

	void PreDraw(FillMode fillmode,BlendMode blendMode);

private:
	DirectXFunc* DXF_;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };

	bool isInitialize_ = false;
};

