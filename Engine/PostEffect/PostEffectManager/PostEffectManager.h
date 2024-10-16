#pragma once
#include"Vector4.h"
#include"DirectXFunc/DirectXFunc.h"
#include"PostEffect/IPipelineStateObject.h"

#include<d3d12.h>
#include<stdint.h>
#include<map>

class PostEffectManager {
public:

	static PostEffectManager* GetInstance();

private:
	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(const PostEffectManager& o) = delete;
	const PostEffectManager& operator=(const PostEffectManager& o) = delete;

public:
	enum EffectType {
		kNone,
		kGrayScale,
		kSepia,
		kVinetting,
		kSmoothing,
		kGaussianFilter,
		kLightOutline,
		kDepthBasedOutline,
		kRadialBlur,
		kDissolve,
		kRandom,
		kHSV,
		_countOfEffectType
	};

	/// <summary>
	/// これまでの描画モデルにポストエフェクトをかける
	/// </summary>
	/// <param name="type">エフェクトタイプ</param>
	/// <param name="isClear">エフェクトを重ね掛けするか</param>
	void PostEffectDraw(EffectType type, bool isClear);

	/// <summary>
	/// デバッグImGui描画
	/// </summary>
	void Debug();

	
	/// <summary>
	/// 初期化（システム
	/// </summary>
	void Initialize();

	//終了処理（システム
	void Finalize();

	
	//システム上の描画前処理
	void SystemPreDraw(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	//SwapCahain前処理
	void PreSwapChainDraw();

	//SwapChainに描画
	void SwapChainDraw();
private:
	DirectXFunc* DXF_;

	//OffScreenRendering* offScreen_ = nullptr;

	//後続演出フラグ
	bool isEffectReqeat_ = false;

	const Vector4 kRenderTClearValue{ 0.0f,0.0f,0.0f,0.0f };

	uint32_t resourceNum_ = 0;
	
	ID3D12Resource* renderTexture_[2] = { nullptr };

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE cHandle_[2];
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle_[2];


	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;


	std::map<EffectType, IPipelineStateObject*>peData_;
};