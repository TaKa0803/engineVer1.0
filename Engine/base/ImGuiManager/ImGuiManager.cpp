#include"ImGuiManager.h"

#pragma region ImGui
#include<imgui_impl_dx12.h>
#include<imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
#pragma endregion
#include<cassert>

using namespace Engine;

ImGuiManager* ImGuiManager::GetInstance()
{
	//インスタンス取得
	static ImGuiManager Instance;
	return &Instance;
}

void ImGuiManager::Initialize(WindowApp* winApp, Engine::DirectXFunc* DXF)
{
	//存在しない場合エラー
	assert(winApp);
	//ポインタ設定
	winApp_ = winApp;

	//存在しない場合エラー
	assert(DXF);
	//ポインタ設定
	DXF_ = DXF;

	//SRVマネージャのインスタンス取得
	SRVM_ = SRVManager::GetInstance();

	//使われていないSRVのハンドル取得
	Handles srvHandle = SRVM_->CreateNewSRVHandles();

#pragma region ImGuiの初期化
	//ImGuiの初期化。詳細はさして重要ではないので解説は省略
	//こういうもんである
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(DXF_->GetDevice(),
		DXF_->GetswapChainDesc().BufferCount,
		DXF_->GetrtvDesc().Format,
		SRVM_->GetSRV(),
		srvHandle.cpu,
		srvHandle.gpu);
#pragma endregion

}

void ImGuiManager::PreUpdate()
{
	//フレームの先頭でImGuiにここからフレームが始まる旨を告げる	
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::PostUpdate()
{
	//ImGuiの内部コマンドを生成する
	ImGui::Render();
}

void ImGuiManager::PreDraw()
{
	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { SRVM_->GetSRV() };
	DXF_->GetCMDList()->SetDescriptorHeaps(1, descriptorHeaps);

}

void ImGuiManager::PostDraw()
{
	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DXF_->GetCMDList());
}

void ImGuiManager::Finalize()
{
	//ImGui開放
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}
