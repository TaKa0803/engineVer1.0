#include "DSVManager.h"
#include"functions/function.h"


Engine::DSVManager* Engine::DSVManager::GetInstance()
{
	//インスタンス取得
	static DSVManager ins;
	return &ins;
}

void Engine::DSVManager::Initialize()
{
	//DirectXFuncのインスタンス取得
	DXF_ = Engine::DirectXFunc::GetInstance();

	//DSV用のヒープでディスクリプタの数は１。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	descriptorHeap_ = CreateDescriptorHeap(DXF_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, maxDSVSize_, false);
	descriptorSize_ = DXF_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
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

void Engine::DSVManager::Finalize()
{
	//メモリの解放
	descriptorHeap_->Release();
	depthStencilResource->Release();
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::DSVManager::GetDescriptorHandle(uint32_t Num)
{
	//引数が仕様数以下の場合
	if (Num < descriptorNum_) {
		//あるものを返却
		return GetCPUDescriptorHandle(descriptorHeap_, descriptorSize_, Num);
	}
	else {
		//新しく作成
		D3D12_CPU_DESCRIPTOR_HANDLE ans = GetCPUDescriptorHandle(descriptorHeap_, descriptorSize_, descriptorNum_);
		//数を増やす
		descriptorNum_++;
		//返却
		return ans;
	}
}


