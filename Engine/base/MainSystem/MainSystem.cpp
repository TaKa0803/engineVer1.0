#include "MainSystem.h"

#include"RandomNum/RandomNum.h"
#include"AudioManager/AudioManager.h"
#include"RTVManager/RTVManager.h"
#include"DSVManager/DSVManager.h"
#include"SingleGameObjects/ObjectPSO.h"
#include"DXC/DXCManager.h"
#include"GameScene/GameScene.h"
#include"SpriteManager/SpriteManager.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"
#include"UAVManager/UAVManager.h"
#include"GVariableManager/GVaribleManager.h"
#include"GvariGroup/GvariGroup.h"


MainSystem* MainSystem::GetInstance() {
	static MainSystem instance;
	return &instance;
}

void MainSystem::Run() {
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//エンジン初期化処理
	Initializes();

	//更新処理
	MainRoop();

	//終了処理
	Finalize();

	//終わり
	CoUninitialize();
}

void MainSystem::Initializes() {
	//windowsアプリケーション
	winApp_ = WindowApp::GetInstance();
	winApp_->Initialize(L"LE2A_07_キクチ",1280,720);

	//DXCマネージャ
	DXCManager* DXC = DXCManager::GetInstance();
	DXC->Initialize();

	//DirectXマネージャ初期化
	DXF_ = DirectXFunc::GetInstance();
	DXF_->Initialize(winApp_);
	DXF_->InitializeOthher();

	//SRVインスタンス取得
	SRVM_ = SRVManager::GetInstance();
	UAVManager::GetInstance()->Initialize();

	//RTVManager::GetInstance()->Initialize();

	//画像関係
	textureManager_= TextureManager::GetInstance();
	textureManager_->Initialize(DXF_);

	PostEffectManager::GetInstance()->Initialize();

	//imgui
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize(winApp_, DXF_);

	//入力
	input_ = Input::GetInstance();
	input_->Initialize(winApp_);
	
	//インスタンシングモデル
	instancingMM_ = InstancingModelManager::GetInstance();

	//乱数クラス
	randomNumClass_ = RandomNumber::GetInstance();
	randomNumClass_->RandomNumberProcessInitialize();
	
	//デルタタイム
	deitaTimer_ = DeltaTimer::GetInstance();
	deitaTimer_->Initialize();

	//音声マネージャ
	AudioManager *audioManager = AudioManager::GetInstance();
	audioManager->Initialize();
	audioManager->LoadAllSoundData();

	//モデルマネージャ初期化
	ModelManager::GetInstance()->Initialize();
	//スプライトマネージャ初期化
	SpriteManager::GetInstance()->Initialize();


	GlobalVariableManager::GetInstance()->LoadAllSaveData();


}

void MainSystem::MainRoop() {



	//モデルデータ関係読み込み
	ModelManager::GetInstance()->LoadAllModels();
	instancingMM_->Initialize(true);


	//音声データ読み込み
	

	//ゲームシーン初期化
	std::unique_ptr<GameScene> gameScene_;
	gameScene_ = std::make_unique<GameScene>();
	gameScene_->Initialize();

	//セットされたデータに保存したものがあればそれを適応する
	GlobalVariableManager::GetInstance()->SetAllLoadData();
	

	while (winApp_->ProcessMessage()) {
		deitaTimer_->Update();
#pragma region 状態更新
		///更新前処理
		//ImGui
		imguiManager_->PreUpdate();

		//キー入力
		input_->Update();
		///=以下更新=//

		DXF_->Update();

		//Gvariの更新
		GlobalVariableManager::GetInstance()->Update();

		//ゲームシーン更新
		gameScene_->Update();


		//==更新終わり==//
		// 
		//更新終わり描画前処理
		imguiManager_->PostUpdate();
#pragma endregion
#pragma region 描画		
		///描画前処理
		//DirectX
		DXF_->PreDraw();	
		//ImGui
		imguiManager_->PreDraw();

		//==以下描画==//	
		gameScene_->Draw();
		//==描画終わり==//

		///描画あと処理
		//imGui
		imguiManager_->PostDraw();
		//DirectX
		DXF_->PostDraw();

		//読み込んだ画像をGPUに送信して削除
		SRVM_->PostInitialize();
#pragma endregion
		
		//シーン終了フラグがtrueの時
		if (IScene::GetLeaveScene()) {
			//シーンを終わる
			break;
		}
	}

}


void MainSystem::Finalize() {

	SpriteManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	///開放処理
	instancingMM_->Finalize();
	SRVM_->Finalize();
	UAVManager::GetInstance()->Finalize();
	RTVManager::GetInstance()->Finalize();
	DSVManager::GetInstance()->Finalize();
	PostEffectManager::GetInstance()->Finalize();

	textureManager_->Finalize();
	imguiManager_->Finalize();
	DXF_->Finalize();
	winApp_->Finalize();

}


