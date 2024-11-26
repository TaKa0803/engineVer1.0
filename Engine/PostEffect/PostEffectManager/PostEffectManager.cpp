#include "PostEffectManager.h"
#include"WinApp/WinApp.h"
#include"SRVManager/SRVManager.h"
#include"functions/function.h"
#include"RTVManager/RTVManager.h"
#include"DSVManager/DSVManager.h"

#include"PostEffect/PEs/PEOffScreen.h"
#include"PostEffect/PEs/PEGrayScale.h"
#include"PostEffect/PEs/PESepia.h"
#include"PostEffect/PEs/PEVignetting.h"
#include"PostEffect/PEs/PESmoothing.h"
#include"PostEffect/PEs/PEGaussianFilter.h"
#include"PostEffect/PEs/PELightOutline.h"
#include"PostEffect/PEs/PEDepthBasedOutline.h"
#include"PostEffect/PEs/PERadialBlur.h"
#include"PostEffect/PEs/PEDissolve.h"
#include"PostEffect/PEs/PERandom.h"
#include"PostEffect/PEs/PEHSVFilter.h"
#include"PostEffect/PEs/PEHighLuminance.h"
#include"PostEffect/PEs/PEBloom.h"

#include"GvariGroup/GvariGroup.h"
#include"ImGuiManager/ImGuiManager.h"
#include<cassert>


PostEffectManager* PostEffectManager::GetInstance()
{
	static PostEffectManager instance;
	return &instance;
}




void PostEffectManager::Initialize()
{
	extractionScene_ = ExtractionScene::GetInstance();
	extractionScene_->Initialize();

	DXF_ = DirectXFunc::GetInstance();

	renderTexture_[0] = DXF_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);
	renderTexture_[1] = DXF_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);

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



	peData_[kNone] = new PEOffScreen();
	peData_[kGrayScale] = new PEGrayScale();
	peData_[kSepia] = new PESepia();
	peData_[kVinetting] = new PEVignetting();
	peData_[kSmoothing] = new PESmoothing();
	peData_[kGaussianFilter] = new PEGaussianFilter();
	peData_[kLightOutline] = new PELightOutline();
	peData_[kDepthBasedOutline] = new PEDepthBasedOutline();
	peData_[kRadialBlur] = new PERadialBlur();
	peData_[kDissolve] = new PEDissolve();
	peData_[kRandom] = new PERandom();
	peData_[kHSV] = new PEHSVFilter();
	peData_[kHighLuminance] = new PEHighLuminace();
	peData_[kBloom] = new PEBloom();


	/*PEDissolve* d = dynamic_cast<PEDissolve*>(peData_[kDissolve]);
	if (d) {

	}*/

	
	effectName_.resize(CountOfEffectType);
	effectName_ = {
	"無し",
	"グレースケール",
	"セピア",
	"ビネット",
	"スムージング",
	"ガウシアンフィルター",
	"ライトアウトライン",
	"深度値アウトライン",
	"ラディアルブラー",
	"ディゾルブ",
	"ランダム",
	"HSV",
	"高光度検出",
	"ブルーム"
	};

	nowEffectName_ = effectName_[kNone];

	std::unique_ptr<GVariGroup> gvg = std::make_unique<GVariGroup>("PostEffect");
	gvg->SetMonitorCombo("エフェクト", &nowEffectName_,effectName_);

}



void PostEffectManager::Finalize()
{
	extractionScene_->Finalize();

	renderTexture_[0]->Release();
	renderTexture_[1]->Release();

	for (auto& data : peData_) {
		data.second->Release();
	}

	peData_.clear();

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
	if (!effective_) {
		return;
	}

	//描画先
	int drawNum;
	if (resourceNum_ == 0) {
		drawNum = 1;
	}
	else {
		drawNum = 0;
	}

	if (type == kBloom) {
		LoadScene2Texture();
		//高光度のみ取得
		PostEffectDraw(kHighLuminance,true);
		//ブラー処理
		PostEffectDraw(kGaussianFilter, true);

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
	if (type >= 0 && type <= CountOfEffectType) {
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
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
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
		//重ね掛けフラグをなくす
		isEffectReqeat_ = false;

		//リソースの切り替え処理を行わない
		//リソース番号のRenderをクリアする
		DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[drawNum], &kRenderTClearValue.x, 0, nullptr);
		

	}
}

void PostEffectManager::GvariEffectDraw()
{
	if (!effective_) {
		return;
	}

	int count = 0;
	for (auto& name : effectName_) {
		if (name == nowEffectName_) {
			break;
		}

		count++;
	}
	gVariType_ = (EffectType)count;

	//描画先
	int drawNum;
	if (resourceNum_ == 0) {
		drawNum = 1;
	}
	else {
		drawNum = 0;
	}

	if (gVariType_ == kBloom) {
		LoadScene2Texture();
		//高光度のみ取得
		PostEffectDraw(kHighLuminance, true);
		//ブラー処理
		PostEffectDraw(kGaussianFilter, true);

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
	DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[drawNum], &kRenderTClearValue.x, 0, nullptr);


	//RenderTextureをSwapchainに描画
	if (gVariType_ >= 0 && gVariType_ <= CountOfEffectType) {
		peData_[gVariType_]->PreDraw();
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
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
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
	if (gVariKeepEffect_) {

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
		//重ね掛けフラグをなくす
		isEffectReqeat_ = false;

		//リソースの切り替え処理を行わない
		//リソース番号のRenderをクリアする
		DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[drawNum], &kRenderTClearValue.x, 0, nullptr);


	}
}

void PostEffectManager::Debug()
{
#ifdef _DEBUG


	ImGui::Begin("PostEffect");

	ImGui::Checkbox("有効", &effective_);

	ImGui::BeginChild("scloll");
	for (auto& pe : peData_) {
		pe.second->Debug();
	}
		
	ImGui::EndChild();
	ImGui::End();
	//}

#endif // _DEBUG

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

void PostEffectManager::LoadScene2Texture()
{
	//現在の画面をコピー
	extractionScene_->LoadSceneTexture(renderTexture_[resourceNum_], cHandle_[resourceNum_], gHandle_[resourceNum_], dsvHandle_);
}
