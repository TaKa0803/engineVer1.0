#include "RTVManager.h"
#include"functions/function.h"

RTVManager* RTVManager::GetInstance()
{
	static RTVManager instance;
	return &instance;
}

void RTVManager::Initialize()
{

	DXF_ = DirectXFunc::GetInstance();

	//RTVようのヒープでディスクリプタの数は２。RTVはSHADER内で触るものではないのでShaderVisibleはfalse
	rtvDescriptorHeap_ = CreateDescriptorHeap(DXF_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxRTVSize_, false);
	descriptorSizeRTV_ = DXF_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

}

void RTVManager::Finalize()
{
	rtvDescriptorHeap_->Release();
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetDescriptorHandle()
{
	
	D3D12_CPU_DESCRIPTOR_HANDLE ans = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, RTVNum_);
	
	RTVNum_++;

	return ans;
}

