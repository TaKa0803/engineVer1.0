#include "UAVManager.h"
#include"functions/function.h"

using namespace Engine;

UAVManager* UAVManager::GetInstance()
{
	//インスタンス取得
	static UAVManager ins;
	return &ins;
}

void UAVManager::Initialize()
{
	//DXFのインスタンス取得
	DXF_ = Engine::DirectXFunc::GetInstance();
	
	//ヒープ作成
	uavDescriptorHeap_ = CreateDescriptorHeap(DXF_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxUAVSize_,true);
	//サイズの値取得
	descriptorSizeUAV_ = DXF_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void UAVManager::Finalize()
{
	//解放処理
	uavDescriptorHeap_->Release();
}

Handles UAVManager::GetDescriptorHandle()
{
	//使っていないハンドルを取得
	Handles ans = {
		GetCPUDescriptorHandle(uavDescriptorHeap_,descriptorSizeUAV_,UAVNum_),
		GetGPUDescriptorHandle(uavDescriptorHeap_,descriptorSizeUAV_,UAVNum_)
	};

	//仕様数を増やす
	UAVNum_++;

	return ans;
}
