#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

class UAVManager {

public://シングルトンパターン
	static UAVManager* GetInstance();

private:
	UAVManager() = default;
	~UAVManager() = default;
	UAVManager(const UAVManager& o) = delete;
	const UAVManager& operator=(const UAVManager& o) = delete;
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	//使っていないhandleを返却
	Handles GetDescriptorHandle();

	ID3D12DescriptorHeap* GetDescriptorHeap() { return uavDescriptorHeap_; }

private:
	DirectXFunc* DXF_;

	ID3D12DescriptorHeap* uavDescriptorHeap_;
	uint32_t descriptorSizeUAV_;

	uint32_t UAVNum_=0;

	const uint32_t maxUAVSize_ = 256;
};