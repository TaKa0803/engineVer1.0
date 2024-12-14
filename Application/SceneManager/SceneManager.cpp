#include"SceneManager/SceneManager.h"
#include"GvariGroup/GvariGroup.h"
#include<imgui.h>


#pragma region シーンのh
#include"Debug/DebugScene.h"
#include"Game/Scene/TitleScene.h"
#include"Game/Scene/InGameScene.h"
#include"Game/Scene/GameOverScene.h"
#include"Game/Scene/GameClearScene.h"
#pragma endregion



SceneManager::SceneManager()
{
	//デバッグ用の値セット
	std::unique_ptr<GlobalVariableGroup>gvg = std::make_unique<GlobalVariableGroup>("SceneManager");
	gvg->SetValue("scene Value", &IScene::GetSceneNo());
	gvg->SetMonitorValue("scene", &scenename_);
	
	sceneName_.clear();
	sceneName_.push_back("DEBUG");
	sceneName_.push_back("TITLE");
	sceneName_.push_back("GAME");
	sceneName_.push_back("GAMEOVER");
	sceneName_.push_back("GAMECLEAR");
	scenename_ = sceneName_[currentSceneNo_].c_str();

}

void SceneManager::Initialize()
{
	//シーンの数取得
	sceneArr_.resize((size_t)SCENE::SceneCount);

	//各シーンの情報設定
	sceneArr_[Debug] = std::make_unique<DebugScnene>();
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[GAME] = std::make_unique<InGameScene>();
	sceneArr_[GAMEOVER] = std::make_unique<GameOverScene>();
	sceneArr_[GAMECLEAR] = std::make_unique<GameClearScene>();

	//初期シーン設定
	IScene::SetSceneNo(TITLE);
}

void SceneManager::Update()
{
	//シーンチェック
	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = IScene::GetSceneNo();

	//シーン変更チェック
	if (prevSceneNo_ != currentSceneNo_)
	{
		//変更していたら		
		//初期化処理
		sceneArr_[currentSceneNo_]->Initialize();
	}

	//デバッグ用現シーン名に変更
	scenename_ = sceneName_[currentSceneNo_].c_str();
	
	//シーン更新処理
	sceneArr_[currentSceneNo_]->Update();
}

void SceneManager::Draw()
{	
	//描画処理
	sceneArr_[currentSceneNo_]->Draw();

}



