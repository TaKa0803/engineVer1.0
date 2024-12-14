#pragma once
#include"Vector4.h"
#include"DirectXFunc/DirectXFunc.h"
#include"PostEffect/IPipelineStateObject.h"
#include"ExtractionScene/ExtractionScene.h"

#include<d3d12.h>
#include<stdint.h>
#include<map>

//ポストエフェクトの管理ツール
class PostEffectManager {
public://**シングルトンパターン**//

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static PostEffectManager* GetInstance();
private:
	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(const PostEffectManager& o) = delete;
	const PostEffectManager& operator=(const PostEffectManager& o) = delete;

public:	//**パブリック関数**//

	//エフェクトタイプ
	enum EffectType {
		kNone,				//無し
		kGrayScale,			//グレイスケール
		kSepia,				//セピア
		kVinetting,			//ビネット
		kSmoothing,			//スムーシング
		kGaussianFilter,	//ガウシアンフィルタ
		kLightOutline,		//ライトアウトライン
		kDepthBasedOutline,	//深度地アウトライン
		kRadialBlur,		//ラディアルブラー
		kDissolve,			//ディゾルブ
		kRandom,			//砂嵐
		kHSV,				//HSV
		kHighLuminance,		//高光度
		kBloom,				//ブルーム
		CountOfEffectType	//エフェクトの数
	};

	/// <summary>
	/// これまでの描画モデルにポストエフェクトをかける
	/// </summary>
	/// <param name="type">エフェクトタイプ</param>
	/// <param name="isClear">エフェクトを重ね掛けするか</param>
	void PostEffectDraw(EffectType type, bool isClear = true);

	/// <summary>
	/// デバッグで指定したエフェクトをかける
	/// </summary>
	void GvariEffectDraw();

	/// <summary>
	/// デバッグImGui描画
	/// </summary>
	void Debug();

	/// <summary>
	/// 初期化（システム
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理（システム
	/// </summary>
	void Finalize();

	/// <summary>
	/// システムの描画前処理
	/// </summary>
	/// <param name="dsvHandle"></param>
	void SystemPreDraw(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	/// <summary>
	/// SwapCahain前処理
	/// </summary>
	void PreSwapChainDraw();

	/// <summary>
	/// SwapChainに描画
	/// </summary>
	void SwapChainDraw();

	/// <summary>
	/// シーン情報を画像として保存するフラグON
	/// </summary>
	void LoadScene2TexFlagActive() { loadScene2Tex_ = true; }

public://**ゲッター**//

	/// <summary>
	/// シーンの画像handleを取得
	/// </summary>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSceneTexture() { return extractionScene_->GetTexHandle(); }

private://**プライベート関数**//

	/// <summary>
	/// シーンを画像として保存
	/// </summary>
	void LoadScene2Texture();

private://**プライベート変数**//

	//DXFのポインタ
	DirectXFunc* DXF_;

	//シーンの画像保存処理
	ExtractionScene* extractionScene_ = nullptr;

	//シーンを画像として保存
	bool loadScene2Tex_ = false;

	//有効処理
	bool effective_ = true;

	//後続演出フラグ
	bool isEffectReqeat_ = false;

	//クリアする色
	const Vector4 kRenderTClearValue{ 0.0f,0.0f,0.0f,0.0f };

	//リソース番号
	uint32_t resourceNum_ = 0;

	//レンダー画像
	ID3D12Resource* renderTexture_[2] = { nullptr };

	//テクスチャデスク
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	
	//各ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cHandle_[2];
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle_[2];

	//深度地ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

	//エフェクトのデータ群
	std::map<EffectType, IPipelineStateObject*>peData_;

public://**Gvari管理でのエフェクト**//

	//指定エフェクト
	EffectType gVariType_ = kNone;

	//エフェクトフラグ
	bool gVariKeepEffect_ = true;

	//現在のエフェクト番号
	std::string nowEffectName_;

	//エフェクトの名前
	std::vector<std::string> effectName_;
};