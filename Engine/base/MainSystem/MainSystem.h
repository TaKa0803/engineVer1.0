#pragma once

#include"WinApp/WinApp.h"
#include"DirectXFunc/DirectXFunc.h"
#include"InstancingModelManager/InstancingModelManager.h"
#include"TextureManager/TextureManager.h"
#include"SingleModelManager/ModelManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include"Input/Input.h"
#include"SRVManager/SRVManager.h"
#include"RandomNum/RandomNum.h"
#include"DeltaTimer/DeltaTimer.h"

class MainSystem {
public://**シングルトンパターン**//
	static MainSystem* GetInstance();
private:
	MainSystem() = default;
	~MainSystem() = default;
	MainSystem(const MainSystem& o) = delete;
	const MainSystem& operator=(const MainSystem& o) = delete;

public://**パブリック関数**//

	/// <summary>
	/// 処理の開始
	/// </summary>
	void Run();

private://**プライベート関数**//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initializes();

	/// <summary>
	/// メインループ処理
	/// </summary>
	void MainRoop();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private://**プライベート変数**//

#pragma region 基板初期化

	//windowsアプリケーション
	WindowApp* winApp_;

	//DirectX
	DirectXFunc* DXF_;

	//ImGui
	ImGuiManager* imguiManager_;

	//texture
	TextureManager* textureManager_;

	//入力
	Input* input_;

	//SRV
	SRVManager* SRVM_;

	//インスタンシングモデル
	InstancingModelManager* instancingMM_;

	//乱数クラス
	RandomNumber* randomNumClass_;

	//デルタタイム
	DeltaTimer* deitaTimer_;
#pragma endregion


};