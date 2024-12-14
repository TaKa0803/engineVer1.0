#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"
#include<stdint.h>

//レンダーターゲットビューの管理マネージャ
class RTVManager {

public://シングルトンパターン
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static RTVManager* GetInstance();
private:
	RTVManager() = default;
	~RTVManager() = default;
	RTVManager(const RTVManager& o) = delete;
	const RTVManager& operator=(const RTVManager& o) = delete;

public://**パブリック関数**//

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

public://**ゲッター**//

	//使っていないhandleを返却
	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle();

	//ディスクリプタヒープ
	ID3D12DescriptorHeap* GetDescriptorHeap() { return rtvDescriptorHeap_; }

private://**プライベート変数**//

	//DirectXFuncのポインタ
	DirectXFunc* DXF_;

	//RTVのディスクリプタヒープ
	ID3D12DescriptorHeap* rtvDescriptorHeap_;
	//ヒープサイズ
	uint32_t descriptorSizeRTV_;

	//RTVの数
	uint32_t RTVNum_ = 0;

	//最大数
	const uint32_t maxRTVSize_ = 6;
};