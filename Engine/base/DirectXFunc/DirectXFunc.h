#pragma once
#include"WinApp/WinApp.h"

#include"Vector4.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<stdint.h>
#include<wrl.h>
#include<chrono>

class OffScreenRendering;

class VignettingPSO;

class DirectXFunc {
public://シングルトンパターン
	static DirectXFunc* GetInstance();

private://シングルトンパターン

	DirectXFunc() = default;
	~DirectXFunc() = default;
	DirectXFunc(const DirectXFunc& o) = delete;
	const DirectXFunc& operator=(const DirectXFunc& o) = delete;


public:	//静的メンバ変数



public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// イニシャライズ
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WindowApp* winApp);

	void InitializeOthher();

	void Update();

	void PrePreDraw();

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

#pragma region ゲッター
	ID3D12Device* GetDevice()const { return device.Get(); }

	ID3D12GraphicsCommandList* GetCMDList()const { return commandList.Get(); }

	DXGI_SWAP_CHAIN_DESC1 GetswapChainDesc()const { return swapChainDesc; }

	D3D12_RENDER_TARGET_VIEW_DESC GetrtvDesc()const { return rtvDesc; }

	
#pragma endregion

private://メンバ関数

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void FixFPSInitialize();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

#pragma region クラス内関数
	void D3D12Devicenitialize();

	void CommandInitialize();

	void SwapChainInitialize();

	void RTVInitialize();

	void DSVInitialize();

	void FenceInitialize();

	void RenderTextureInitialize();
#pragma endregion

private://メンバ変数
#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
#endif // _DEBUG

	//ポインタ
	WindowApp* winApp_ = nullptr;



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

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };


	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	ID3D12DescriptorHeap* rtvDescriptorHeap;
	uint32_t descriptorSizeRTV;

	ID3D12Resource* depthStencilResource;
	ID3D12DescriptorHeap* dsvDescriptorHeap;
	uint32_t descriptorSizeDSV;



	OffScreenRendering* offScreen_ = nullptr;
	//VignettingPSO* offScreen_ = nullptr;


	const Vector4 kRenderTargetClearValue={ 1.0f,0.0f,0.0f,1.0f };
	ID3D12Resource* renderTextureResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE cHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle_;

	ID3D12Resource* texResource_;

	//フェンス
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	//イベント
	HANDLE fenceEvent;




	//fps固定用
	std::chrono::steady_clock::time_point reference_;

	//開放チェックエラーで実行を止めるか	
	const bool isAssertForgetReleasing_ = true;


};