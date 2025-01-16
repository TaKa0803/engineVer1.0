#include"SceneManager/SceneManager.h"
#include"GlobalVariable/Manager/GlobalVaribleManager.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"

#include"SceneFactory/SceneFactory.h"

#pragma region シーンのh
#include"Debug/DebugScene.h"
#include"Game/Scene/TitleScene.h"
#include"Game/Scene/InGameScene.h"
#include"Game/Scene/GameOverScene.h"
#include"Game/Scene/GameClearScene.h"
#pragma endregion

#include<cassert>


SceneManager::SceneManager(){}

void SceneManager::Initialize()
{
	//初期シーン設定
	IScene::SetScene(SCENE::TITLE);
}

void SceneManager::Update()
{
	//シーンチェック
	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = static_cast<int>(IScene::GetSceneNo());

	//シーン変更チェック
	if (prevSceneNo_ != currentSceneNo_)
	{
		//変更していたら		

		//過去のデバッグデータをクリア
		GlobalVariableManager::GetInstance()->ClearSetData();

		//デバッグ用の値セット
		std::unique_ptr<GlobalVariableGroup>gvg = std::make_unique<GlobalVariableGroup>("SceneManager");
		gvg->SetValue("シーン番号", &IScene::GetSceneNo());
		gvg->SetMonitorValue("シーン名", &scenename_);

		//シーンを読み込み
		scene_ = SceneFactory::CreateScene((SCENE)currentSceneNo_);
		scene_->Initialize();
	}

	//デバッグ用に現シーン名に変更
	scenename_ = scene_->name_;
	
	//シーン更新処理
	scene_->Update();
}

void SceneManager::Draw()
{	
	//描画処理
	scene_->Draw();
}



