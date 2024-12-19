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

#include"GlobalVariable/Group/GlobalVariableGroup.h"
#include"ImGuiManager/ImGuiManager.h"
#include<cassert>


PostEffectManager* PostEffectManager::GetInstance()
{
	//インスタンス取得
	static PostEffectManager instance;
	return &instance;
}

void PostEffectManager::Initialize()
{
	//シーンを画像を保存する処理クラスの生成と初期化
	extractionScene_ = ExtractionScene::GetInstance();
	extractionScene_->Initialize();

	//DirectXFuncのインスタンス生成
	DXF_ = DirectXFunc::GetInstance();

	//レンダーテクスチャ生成
	renderTexture_[0] = DXF_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);
	renderTexture_[1] = DXF_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);

	//デスクデータ生成
	D3D12_RENDER_TARGET_VIEW_DESC rtDesc{};
	rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//RTVマネージャのインスタンス取得
	RTVManager* RTVM = RTVManager::GetInstance();

	//二枚のハンドルを取得
	cHandle_[0] = RTVM->GetDescriptorHandle();
	cHandle_[1] = RTVM->GetDescriptorHandle();

	//RTV作成
	DXF_->GetDevice()->CreateRenderTargetView(renderTexture_[0], &rtDesc, cHandle_[0]);
	DXF_->GetDevice()->CreateRenderTargetView(renderTexture_[1], &rtDesc, cHandle_[1]);

	//SRVの設定
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	//２枚のGPUハンドル取得
	gHandle_[0] = SRVManager::CreateSRV(renderTexture_[0], renderTextureSrvDesc).gpu;
	gHandle_[1] = SRVManager::CreateSRV(renderTexture_[1], renderTextureSrvDesc).gpu;

	//各PEクラスの生成
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

	//デバッグ用に名前群作成
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

	//とりあえず通常
	nowEffectName_ = effectName_[kNone];

	//デバッグ値追加
	std::unique_ptr<GlobalVariableGroup> gvg = std::make_unique<GlobalVariableGroup>("PostEffect");
	gvg->SetMonitorCombo("エフェクト", &nowEffectName_,effectName_);
}



void PostEffectManager::Finalize()
{

	//各解放処理
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
	//深度ハンドル取得
	dsvHandle_ = dsvHandle;

	//後続にとエフェクトをかけるフラグをON
	isEffectReqeat_ = true;

	//リソース番号設定
	resourceNum_ = 0;

	//レンダーターゲットを設定
	DXF_->GetCMDList()->OMSetRenderTargets(1, &cHandle_[resourceNum_], false, &dsvHandle);

	//指定した色で画面全体をクリアする
	float clearColor[] = { kRenderTClearValue.x,kRenderTClearValue.y,kRenderTClearValue.z,kRenderTClearValue.w };
	
	//指定した深度で画面全体をクリアする
	DXF_->GetCMDList()->ClearRenderTargetView(cHandle_[resourceNum_], clearColor, 0, nullptr);

}

void PostEffectManager::PostEffectDraw(EffectType type, bool isKeepEffect)
{
	//有効処理がOFFの場合変える
	if (!effective_) {
		return;
	}

	//描画先の番号取得
	int drawNum;
	if (resourceNum_ == 0) {
		drawNum = 1;
	}
	else {
		drawNum = 0;
	}

	//ブルームの場合特別処理
	if (type == kBloom) {
		//現シーン画像をコピーして保存
		LoadScene2Texture();
		//高光度のみ取得
		PostEffectDraw(kHighLuminance,true);
		//ブラー処理
		PostEffectDraw(kGaussianFilter);
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
		//範囲外なら何もしない
		peData_[kNone]->PreDraw();
	}
	//画像設定
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(0, gHandle_[resourceNum_]);
	//描画
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
	//無効なら早期リターン
	if (!effective_) {
		return;
	}
	//カウント
	int count = 0;
	//名前が合致したら終了
	for (auto& name : effectName_) {
		if (name == nowEffectName_) {
			break;
		}
		count++;
	}

	//合致したタイプに変更
	gVariType_ = (EffectType)count;

	//描画先の数字を取得
	int drawNum;
	if (resourceNum_ == 0) {
		drawNum = 1;
	}
	else {
		drawNum = 0;
	}

	//ブルームの場合特別処理
	if (gVariType_ == kBloom) {
		//現在シーン画像を別途保存
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
	//デバッグウィンドウ表示
	ImGui::Begin("PostEffect");
	ImGui::Checkbox("有効", &effective_);
	ImGui::BeginChild("scloll");
	for (auto& pe : peData_) {
		pe.second->Debug();
	}	
	ImGui::EndChild();
	ImGui::End();
#endif // _DEBUG
}



void PostEffectManager::PreSwapChainDraw()
{
	//シーンを画像として読み込むフラグがある場合
	if (loadScene2Tex_) {
		//フラグをオフ
		loadScene2Tex_ = false;
		//シーンを画像にする処理
		LoadScene2Texture();
	}

	//フラグがオフの場合一度処理を挟む必要がある
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

	//描画前処理
	peData_[kNone]->PreDraw();

	//画像を
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
