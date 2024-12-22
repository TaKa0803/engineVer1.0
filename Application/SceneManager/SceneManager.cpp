#include"SceneManager/SceneManager.h"
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


SceneManager::SceneManager()
{
	sceneArr_.resize((int)SCENE::SceneCount);
	sceneArr_[(int)SCENE::Debug] = std::make_unique<DebugScnene>();
	//sceneArr_[(int)SCENE::TITLE] = std::make_unique<TitleScene>();
	//sceneArr_[(int)SCENE::GAME] = std::make_unique<InGameScene>();
	//sceneArr_[(int)SCENE::GAMEOVER] = std::make_unique<GameOverScene>();
	//sceneArr_[(int)SCENE::GAMECLEAR] = std::make_unique<GameClearScene>();


	//デバッグ用の値セット
	std::unique_ptr<GlobalVariableGroup>gvg = std::make_unique<GlobalVariableGroup>("SceneManager");
	gvg->SetValue("シーン番号", &IScene::GetSceneNo());
	gvg->SetMonitorValue("シーン名", &scenename_);
}

void SceneManager::Initialize()
{
	//初期シーン設定
	IScene::SetScene(SCENE::Debug);
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
		//sceneArr_ = SceneFactory::CreateScene((SCENE)currentSceneNo_);
		//if (sceneArr_ == nullptr) {
		//	assert(false);
		//}
		// 
		//初期化処理
		sceneArr_[currentSceneNo_]->Initialize();
	}

	//デバッグ用現シーン名に変更
	scenename_ = sceneArr_[currentSceneNo_]->name_;
	
	//シーン更新処理
	sceneArr_[currentSceneNo_]->Update();
}

void SceneManager::Draw()
{	
	//描画処理
	sceneArr_[currentSceneNo_]->Draw();
}



