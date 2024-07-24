#include "UAVManager.h"
#include"functions/function.h"

UAVManager* UAVManager::GetInstance()
{
	static UAVManager ins;
	return &ins;
}

void UAVManager::Initialize()
{
	DXF_ = DirectXFunc::GetInstance();
	
	uavDescriptorHeap_ = CreateDescriptorHeap(DXF_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxUAVSize_,false);
	descriptorSizeUAV_ = DXF_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void UAVManager::Finalize()
{
	uavDescriptorHeap_->Release();
}

Handles UAVManager::GetDescriptorHandle()
{
	Handles ans = {
		GetCPUDescriptorHandle(uavDescriptorHeap_,descriptorSizeUAV_,UAVNum_),
		GetGPUDescriptorHandle(uavDescriptorHeap_,descriptorSizeUAV_,UAVNum_)
	};

	UAVNum_++;

	return ans;
}
