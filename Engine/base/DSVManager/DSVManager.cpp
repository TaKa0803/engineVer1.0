#include "DSVManager.h"
#include"functions/function.h"


DSVManager* DSVManager::GetInstance()
{
	static DSVManager ins;
	return &ins;
}

void DSVManager::Initialize()
{
	DXF_ = DirectXFunc::GetInstance();


	//DSV用のヒープでディスクリプタの数は１。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(DXF_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, maxDSVSize_, false);
	descriptorSizeDSV_ = DXF_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
#pragma region DSV
	//DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource = CreateDepthStencilTextureResource(DXF_->GetDevice(), WindowApp::kClientWidth, WindowApp::kClientHeight);
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//Format基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//2dTexture

	D3D12_CPU_DESCRIPTOR_HANDLE dsvh = GetDescriptorHandle(0);

	//DSVHEapの先頭にDSVを作る
	DXF_->GetDevice()->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvh);
#pragma endregion
}

void DSVManager::Finalize()
{
	dsvDescriptorHeap_->Release();
	depthStencilResource->Release();
}

D3D12_CPU_DESCRIPTOR_HANDLE DSVManager::GetDescriptorHandle(uint32_t Num)
{

	if (Num < DSVNum_) {
		return GetCPUDescriptorHandle(dsvDescriptorHeap_, descriptorSizeDSV_, Num);
	}
	else {
		D3D12_CPU_DESCRIPTOR_HANDLE ans = GetCPUDescriptorHandle(dsvDescriptorHeap_, descriptorSizeDSV_, DSVNum_);

		DSVNum_++;

		return ans;
	}
}


