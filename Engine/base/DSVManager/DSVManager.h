#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"
#include<stdint.h>

class DSVManager {

public://シングルトンパターン
	static DSVManager* GetInstance();


private:
	DSVManager() = default;
	~DSVManager() = default;
	DSVManager(const DSVManager& o) = delete;
	const DSVManager& operator=(const DSVManager& o) = delete;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

public:

	//使っていないhandleを返却
	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t num);

	ID3D12DescriptorHeap* GetDescriptorHeap() { return dsvDescriptorHeap_; }

	ID3D12Resource* GetdepthStancilResource() { return depthStencilResource; }
private:

	DirectXFunc* DXF_;

	ID3D12DescriptorHeap* dsvDescriptorHeap_=nullptr;
	uint32_t descriptorSizeDSV_;
	ID3D12Resource* depthStencilResource = nullptr;

	uint32_t DSVNum_ = 0;;

	const uint32_t maxDSVSize_ = 1;
};