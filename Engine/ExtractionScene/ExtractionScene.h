#pragma once
#include"Vector4.h"
#include"DirectXFunc/DirectXFunc.h"
#include"PostEffect/PEs/PEOffScreen.h"

//そのシーンを画像として保持する処理
class ExtractionScene {

public:	//**シングルトンパターン**//

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static ExtractionScene* GetInstance();
private:
	ExtractionScene() = default;
	~ExtractionScene() = default;
	ExtractionScene(const ExtractionScene& o) = delete;
	const ExtractionScene& operator=(const ExtractionScene& o) = delete;

public:	//**パブリック関数**//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// そのシーンを画像として保持
	/// </summary>
	/// <param name="res">リソース</param>
	/// <param name="handle">CPUハンドル</param>
	/// <param name="gHandle">GPUハンドル</param>
	/// <param name="dsv"></param>
	void LoadSceneTexture(ID3D12Resource* res, D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE gHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsv);

	/// <summary>
	/// 画像のGPUハンドル取得
	/// </summary>
	/// <returns>GPUハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexHandle() { return gHandle_; }

private: //**プライベート変数**//

	//DirectXFuncのポインタ
	DirectXFunc* DXF_ = nullptr;

	//変化無しでシーンをコピーする処理クラス
	std::unique_ptr<PEOffScreen>peOffScreen_ = nullptr;

	//レンダーテクスチャ
	ID3D12Resource* renderTexture_=nullptr;

	//レンダーテクスチャのデスク
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	//CPUhandle
	D3D12_CPU_DESCRIPTOR_HANDLE cHandle_{};
	//GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle_{};

	//クリアする色
	const Vector4 kRenderTClearValue{ 0.0f,0.0f,0.0f,0.0f };
};