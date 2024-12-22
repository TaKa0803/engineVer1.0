#pragma once
#include"WinApp/WinApp.h"

#include"Vector4.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<stdint.h>
#include<wrl.h>
#include<chrono>

//DirectXに関する処理をまとめたクラス
class DirectXFunc {

public: //**シングルトンパターン**//
	static DirectXFunc* GetInstance();

private: //**シングルトンパターン**//
	DirectXFunc() = default;
	~DirectXFunc() = default;
	DirectXFunc(const DirectXFunc& o) = delete;
	const DirectXFunc& operator=(const DirectXFunc& o) = delete;

public: //**パブリック関数**//

	/// <summary>
	/// イニシャライズ
	/// </summary>
	/// <param name="winApp">winAppのポインタ</param>
	void Initialize(WindowApp* winApp);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// キックコマンド
	/// </summary>
	void KickCommand();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// レンダーテクスチャの作成
	/// </summary>
	/// <param name="format">フォーマット</param>
	/// <param name="clearColor">クリアする色</param>
	/// <returns>リソースのポインタ</returns>
	ID3D12Resource* CreateRenderTextureResource( DXGI_FORMAT format, const Vector4& clearColor);

public: //**ゲッター**//

	//デバイス取得
	ID3D12Device* GetDevice()const { return device.Get(); }

	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCMDList()const { return commandList.Get(); }

	//スワップチェインデスク取得
	DXGI_SWAP_CHAIN_DESC1 GetswapChainDesc()const { return swapChainDesc; }

	//RTVDesc取得
	D3D12_RENDER_TARGET_VIEW_DESC GetrtvDesc()const { return rtvDesc; }

	//スワップチェイン取得
	IDXGISwapChain4* GetSwapChain() { return swapChain.Get(); }

private: //**プライベート関数**//

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void FixFPSInitialize();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();


	/// <summary>
	/// デバイス初期化
	/// </summary>
	void D3D12DeviceInit();

	/// <summary>
	/// コマンドリスト初期化
	/// </summary>
	void CommandInit();

	/// <summary>
	/// スワップチェイン初期化
	/// </summary>
	void SwapChainInit();

	/// <summary>
	/// レンダーターゲットビュー初期化
	/// </summary>
	void RTVInit();

	/// <summary>
	/// デプスステンシルビュー初期化
	/// </summary>
	void DSVInit();

	/// <summary>
	/// フェンス初期化
	/// </summary>
	void FenceInit();

private: //**メンバ変数**//

	//コムポインタの名前省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

#ifdef _DEBUG
	//デバッグのみ使用
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
#endif // _DEBUG

	//ポインタ
	WindowApp* winApp_ = nullptr;

	//アダプター
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//dxgiファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//デバイス
	ComPtr <ID3D12Device> device = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	//スワップチェインのデスク
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };

	//レンダーターゲットビューのデスク
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//RTVのハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	//フェンス
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	//イベント
	HANDLE fenceEvent;

	//fps固定用
	std::chrono::steady_clock::time_point reference_;

	//開放チェックエラーで実行を止めるか	
	const bool isAssertForgetReleasing_ = false;
};