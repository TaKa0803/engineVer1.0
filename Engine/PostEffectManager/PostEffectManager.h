#pragma once
#include"Vector4.h"
#include"DirectXFunc/DirectXFunc.h"
#include"OffScreanPipeline/OffScreanPipeline.h"
#include<d3d12.h>
#include<stdint.h>

class PostEffectManager {
public:

	static PostEffectManager* GetInstance();

private:
	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(const PostEffectManager& o) = delete;
	const PostEffectManager& operator=(const PostEffectManager& o) = delete;

public:

	void Initialize();

	void Finalize();

	enum EffectType {
		kNone,
		kGrayScale,
		kSepia,
		kVinetting,
		kSmoothing,
		kGaussianFilter,
		_countOfEffectType
	};

	void SystemPreDraw(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="type"></param>
	/// <param name="isClear">エフェクトを重ね掛けするか</param>
	void PostEffectDraw(EffectType type, bool isClear);

	void PreSwapChainDraw();

	void SwapChainDraw();
private:
	DirectXFunc* DXF_;

	OffScreenRendering* offScreen_ = nullptr;

	//後続演出フラグ
	bool isEffectReqeat_ = false;

	const Vector4 kRenderTClearValue{ 0.0f,0.0f,0.0f,1.0f };

	uint32_t resourceNum_ = 0;
	
	ID3D12Resource* renderTexture_[2] = { nullptr };

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE cHandle_[2];
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle_[2];


	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;
};