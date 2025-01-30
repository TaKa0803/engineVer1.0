#include "ExtractionScene.h"
#include"RTVManager/RTVManager.h"
#include"SRVManager/SRVManager.h"
#include"DSVManager/DSVManager.h"

using namespace Engine;

ExtractionScene* ExtractionScene::GetInstance()
{
	//インスタンス取得
	static ExtractionScene ins;
	return&ins;
}

void ExtractionScene::Initialize()
{
	//DirectXFuncのインスタンス取得
	DXF_ = Engine::DirectXFunc::GetInstance();
	//レンダー画像作成
	renderTexture_ = DXF_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTClearValue);

	//デスクに値を設定
	D3D12_RENDER_TARGET_VIEW_DESC rtDesc{};
	rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//RTVマネージャのインスタンス取得
	Engine::RTVManager* RTVM = Engine::RTVManager::GetInstance();
	//使われていないCPUハンドルを取得
	cHandle_ = RTVM->GetDescriptorHandle();
	
	//レンダー画像作成
	DXF_->GetDevice()->CreateRenderTargetView(renderTexture_, &rtDesc, cHandle_);

	//SRVの設定
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	//SRVを作成してGPUハンドル取得
	gHandle_ = SRVManager::CreateSRV(renderTexture_, renderTextureSrvDesc).gpu;

	//シーンコピークラスの生成
	peOffScreen_ = std::make_unique<PEOffScreen>();

	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};
	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_;
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore =D3D12_RESOURCE_STATE_RENDER_TARGET;
	////遷移後のResourceState
	barrier_.Transition.StateAfter =  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);
}

void ExtractionScene::Finalize()
{
	//解放処理
	renderTexture_->Release();
	peOffScreen_->Release();
}

void ExtractionScene::LoadSceneTexture(ID3D12Resource*res,D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE gHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
{
	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};
	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = res;
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////遷移後のResourceState
	barrier_.Transition.StateAfter =  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);


	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_;
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	//depthのバリア設定
	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = Engine::DSVManager::GetInstance()->GetdepthStancilResource();
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	//描画先の設定
	DXF_->GetCMDList()->OMSetRenderTargets(1, &cHandle_, false, &dsv);
	//画像をクリア
	DXF_->GetCMDList()->ClearRenderTargetView(cHandle_, &kRenderTClearValue.x, 0, nullptr);
	//するピーンコピー処理のパイプライン呼び出し
	peOffScreen_->PreDraw();
	//画像をセット
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(0, gHandle);
	//描画処理
	DXF_->GetCMDList()->DrawInstanced(3, 1, 0, 0);

	//depthのバリア設定
	////Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = Engine::DSVManager::GetInstance()->GetdepthStancilResource();
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTexture_;
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	////バリアを張る対象のリソース、現在のバックバッファに対して行う
	barrier_.Transition.pResource = res;
	////遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	////遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////TransitionBarrierを張る
	DXF_->GetCMDList()->ResourceBarrier(1, &barrier_);

	//レンダー置き換え
	DXF_->GetCMDList()->OMSetRenderTargets(1, &handle, false, &dsv);

}
