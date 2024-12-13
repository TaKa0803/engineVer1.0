#pragma once

#include"WinApp/WinApp.h"
#include"DirectXFunc/DirectXFunc.h"
#include"TextureManager/TextureManager.h"
#include"SRVManager/SRVManager.h"

#include"imgui.h"

//ImGuiの処理クラス
class ImGuiManager
{
public:	//**シングルトンパターン**//
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();
private://シングルトンパターン
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager& o) = delete;
	const ImGuiManager& operator=(const ImGuiManager& o) = delete;

public:	//**パブリック関数**//

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WindowApp* winApp, DirectXFunc* DXF);

	/// <summary>
	/// 更新最初に処理
	/// </summary>
	void PreUpdate();

	/// <summary>
	/// 更新後描画前に処理
	/// </summary>
	void PostUpdate();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 開放処理
	/// </summary>
	void Finalize();

private: //**プライベート変数**//

	//ポインタ

	//winAppのポインタ
	WindowApp* winApp_ = nullptr;

	//DirectXFuncのポインタ
	DirectXFunc* DXF_ = nullptr;

	//テクスチャマネージャのポインタ
	TextureManager* textureManager_ = nullptr;

	//SRVマネージャのポインタ
	SRVManager* SRVM_=nullptr;

	//ディスクリプタの数
	const size_t kNumDescriptors = 256;
};

