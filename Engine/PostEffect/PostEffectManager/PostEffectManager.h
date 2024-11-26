#pragma once
#include"Vector4.h"
#include"DirectXFunc/DirectXFunc.h"
#include"PostEffect/IPipelineStateObject.h"
#include"ExtractionScene/ExtractionScene.h"

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
	//エフェクトタイプ
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
		kHighLuminance,
		kBloom,
		CountOfEffectType
	};



	/// <summary>
	/// これまでの描画モデルにポストエフェクトをかける
	/// </summary>
	/// <param name="type">エフェクトタイプ</param>
	/// <param name="isClear">エフェクトを重ね掛けするか</param>
	void PostEffectDraw(EffectType type, bool isClear);

	//GlovalVで指定したエフェクトをかける
	void GvariEffectDraw();

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

	/// <summary>
	/// シーンを画像として保存
	/// </summary>
	void LoadScene2Texture();
public:

	/// <summary>
	/// シーンの画像handleを取得
	/// </summary>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSceneTexture() { return extractionScene_->GetTexHandle(); }

private:
	DirectXFunc* DXF_;

	ExtractionScene* extractionScene_=nullptr;

	
	bool effective_ = true;

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

public://Gvari管理でのエフェクト

	EffectType gVariType_ = kNone;

	bool gVariKeepEffect_ = true;

	//現在のエフェクト番号
	std::string nowEffectName_;

	std::vector<std::string> effectName_;
};