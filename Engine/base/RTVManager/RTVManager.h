#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"
#include<stdint.h>

class RTVManager {

public://シングルトンパターン
	static RTVManager* GetInstance();


private:
	RTVManager() = default;
	~RTVManager() = default;
	RTVManager(const RTVManager& o) = delete;
	const RTVManager& operator=(const RTVManager& o) = delete;

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
	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle();

	ID3D12DescriptorHeap* GetDescriptorHeap() { return rtvDescriptorHeap_; }
	
private:

	DirectXFunc* DXF_;

	ID3D12DescriptorHeap* rtvDescriptorHeap_;
	uint32_t descriptorSizeRTV_;

	uint32_t RTVNum_;

	const uint32_t maxRTVSize_ = 6;
};