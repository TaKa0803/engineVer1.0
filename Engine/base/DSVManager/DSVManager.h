#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"
#include<stdint.h>

class DSVManager {

public:	//**シングルトンパターン**//
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static DSVManager* GetInstance();
private:
	DSVManager() = default;
	~DSVManager() = default;
	DSVManager(const DSVManager& o) = delete;
	const DSVManager& operator=(const DSVManager& o) = delete;

public:	//**パブリック関数**//

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

public:	//**ゲッター**//

	//使っていないhandleを返却
	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t num);
	
	//ディスクリプタヒープ取得
	ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap_; }
	
	//深度地リソース取得
	ID3D12Resource* GetdepthStancilResource() { return depthStencilResource; }

private: //**プライベート変数**//

	//DXFのポインタ
	DirectXFunc* DXF_;

	//ヒープ
	ID3D12DescriptorHeap* descriptorHeap_=nullptr;
	//サイズ
	uint32_t descriptorSize_;
	//リソース
	ID3D12Resource* depthStencilResource = nullptr;

	//使用数
	uint32_t descriptorNum_ = 0;

	//最大仕様数
	const uint32_t maxDSVSize_ = 1;
};