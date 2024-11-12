#pragma once
#include"DirectXFunc/DirectXFunc.h"

#include<string>

class SimplePostEffect {

	void Initialize(const std::wstring effectName);

private:

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;
	//パイプライン
	ID3D12PipelineState* psoState_ = nullptr;

};