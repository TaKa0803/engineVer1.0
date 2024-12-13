#include "AppScene.h"
#include"GVariableManager/GlobalVaribleManager.h"

AppScene::AppScene()
{
	//シーンマネージャ生成
	sceneManager =std::make_unique<SceneManager>();
}

void AppScene::Initialize()
{
	//シーンマネージャの初期化
	sceneManager->Initialize();

	//セットされたデータに保存したものがあればそれを適応する
	GlobalVariableManager::GetInstance()->SetLoadAllData();
}

void AppScene::Update()
{
	//シーンマネージャ更新
	sceneManager->Update();
}

void AppScene::Draw()
{
	//シーンマネージャ描画
	sceneManager->Draw();
}
