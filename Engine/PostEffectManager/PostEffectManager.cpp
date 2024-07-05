#include "PostEffectManager.h"
#include"WinApp/WinApp.h"
#include"SRVManager/SRVManager.h"
#include"functions/function.h"
#include"RTVManager/RTVManager.h"
#include"DSVManager/DSVManager.h"

#include"PostEffects/PEs/PEOffScreen.h"
#include"PostEffects/PEs/PEGrayScale.h"
#include"PostEffects/PEs/PESepia.h"
#include"PostEffects/PEs/PEVignetting.h"
#include"PostEffects/PEs/PESmoothing.h"
#include"PostEffects/PEs/PEGaussianFilter.h"
#include"PostEffects/PEs/PELightOutline.h"
#include"PostEffects/PEs/PEDepthBasedOutline.h"

#include<cassert>


PostEffectManager* PostEffectManager::GetInstance()
{
	static PostEffectManager instance;
	return &instance;
}


ID3D12Resource* CreateRenderTextureResource(ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {


	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;											//Textureの幅
	resourceDesc.Height = height;										//Textureの高さ
	resourceDesc.MipLevels = 1;											//mipmapの数
	resourceDesc.DepthOrArraySize = 1;									//奥行き　or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;									//サンプリングカウント、１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;		//RenderTargetとして使う通知


	//Heap生成
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,	//これから描画することを前提としたTextureなのでRenderTargetとして使うことから始める
		&clearValue,						//clear最適値、ClearRenderTargetをこの色でClearするようにする。最適化されているので最速
		IID_PPV_ARGS(&resource)
	);

	assert(SUCCEEDED(hr));

	return resource;
}


void PostEffectManager::Initialize()
{
	DXF_ = DirectXFunc::GetInstance();

	renderTexture_[0] = CreateRenderTextureResource(DXF_->GetDevice(), WindowApp::kClientWidth, WindowApp::kClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);
	renderTexture_[1] = CreateRenderTextureResource(DXF_->GetDevice(), WindowApp::kClientWidth, WindowApp::kClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);

	D3D12_RENDER_TARGET_VIEW_DESC rtDesc{};
	rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;


	RTVManager* RTVM = RTVManager::GetInstance();


	cHandle_[0] = RTVM->GetDescriptorHandle();
	cHandle_[1] = RTVM->GetDescriptorHandle();

	//gHandle_ = GetGPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 2);

	DXF_->GetDevice()->CreateRenderTargetView(renderTexture_[0], &rtDesc, cHandle_[0]);
	DXF_->GetDevice()->CreateRenderTargetView(renderTexture_[1], &rtDesc, cHandle_[1]);


	//SRVの設定
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	gHandle_[0] = SRVManager::CreateSRV(renderTexture_[0], renderTextureSrvDesc).gpu;
	gHandle_[1] = SRVManager::CreateSRV(renderTexture_[1], renderTextureSrvDesc).gpu;


	//offScreen_ = new OffScreenRendering();
	//offScreen_ = new VignettingPSO();
	//offScreen_->Initialize();


	peData_[kNone] = new PEOffScreen();
	peData_[kNone]->Initialize();

	peData_[kGrayScale] = new PEGrayScale();
	peData_[kGrayScale]->Initialize();

	peData_[kSepia] = new PESepia();
	peData_[kSepia]->Initialize();

	peData_[kVinetting] = new PEVignetting();
	peData_[kVinetting]->Initialize();

	peData_[kSmoothing] = new PESmoothing();
	peData_[kSmoothing]->Initialize();

	peData_[kGaussianFilter] = new PEGaussianFilter();
	peData_[kGaussianFilter]->Initialize();

	peData_[kLightOutline] = new PELightOutline();
	peData_[kLightOutline]->Initialize();

	peData_[kDepthBasedOutline] = new PEDepthBasedOutline();
	peData_[kDepthBasedOutline]->Initialize();

}

void PostEffectManager::Finalize()
{
	renderTexture_[0]->Release();
	renderTexture_[1]->Release();


	for (auto& data : peData_) {
		data.second->Release();
	}
	
}


void PostEffectManager::SystemPreDraw(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{

	dsvHandle_ = dsvHandle;

	isEffectReqeat_ = true;

	resourceNum_ = 0;

	DXF_->GetCMDList()->OMSetRenderTargets(1, &cHandle_[resourceNum_], false, &dsvHandle);
#pragma endregion
	//指定した色で画面全体をクリアする
	float clearColor[] = { kRenderTClearValue.x,kRenderTClearValue.y,kRenderTClearValue.z,kRenderTClearValue.w };
	//指定した深度で画面全体をクリアする
	DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[resourceNum_], clearColor, 0, nullptr);

}

void PostEffectManager::PostEffectDraw(EffectType type, bool isKeepEffect)
{
	//描画先
	int drawNum;
	if (resourceNum_ == 0) {
		drawNum = 1;
	}
	else {
		drawNum = 0;
	}

#pragma region 書いているところをリソースにして片方に描画
	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};

	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_[resourceNum_];
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	//depthのバリア設定
	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = DSVManager::GetInstance()->GetdepthStancilResource();
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

#pragma region 描画先を指定して描画
	//描画先の設定
	DXF_->GetCMDList()->OMSetRenderTargets(1, &cHandle_[drawNum], false, &dsvHandle_);

	//指定した色で画面全体をクリアする
	//指定した深度で画面全体をクリアする
	//DXF_->GetCMDList()->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	if (isEffectReqeat_) {
		DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[drawNum], &kRenderTClearValue.x, 0, nullptr);
	}
	//RenderTextureをSwapchainに描画
	//offScreen_->materialData_->type = (int)type;
	//offScreen_->PreDraw();
	if (type >= 0 && type <= _countOfEffectType) {
		peData_[type]->PreDraw();
	}
	else {
		peData_[kNone]->PreDraw();
	}
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(0, gHandle_[resourceNum_]);
	DXF_->GetCMDList()->DrawInstanced(3, 1, 0, 0);
#pragma endregion
	//depthのバリア設定
	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = DSVManager::GetInstance()->GetdepthStancilResource();
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore =  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter =D3D12_RESOURCE_STATE_DEPTH_WRITE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_[resourceNum_];
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);
#pragma endregion

	//後続エフェクトもかける場合の処理
	if (isKeepEffect) {

		isEffectReqeat_ = true;
		//書き込み先を変更してあるので各設定のみ変更
		if (resourceNum_ == 0) {
			resourceNum_ = 1;
		}
		else {
			resourceNum_ = 0;
		}

		

	}
	else {
		isEffectReqeat_ = false;
		//描画先の設定
		DXF_->GetCMDList()->OMSetRenderTargets(1, &cHandle_[resourceNum_], false, &dsvHandle_);

		//指定した色で画面全体をクリアする
		//指定した深度で画面全体をクリアする
		//DXF_->GetCMDList()->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[resourceNum_], &kRenderTClearValue.x, 0, nullptr);

	}
}

void PostEffectManager::Debug(EffectType type)
{
	peData_[type]->Debug();
}

void PostEffectManager::PreSwapChainDraw()
{
	if (!isEffectReqeat_) {
		PostEffectDraw(kNone, true);
	}
}

void PostEffectManager::SwapChainDraw()
{
	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};

	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_[resourceNum_];
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	//RenderTextureをSwapchainに描画
	//offScreen_->PreDraw();
	
	peData_[kNone]->PreDraw();
	

	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(0, gHandle_[resourceNum_]);
	DXF_->GetCMDList()->DrawInstanced(3, 1, 0, 0);

	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_[resourceNum_];
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);
}
