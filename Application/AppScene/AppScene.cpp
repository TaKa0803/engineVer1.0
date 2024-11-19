#include "AppScene.h"
#include"GVariableManager/GVaribleManager.h"

AppScene::AppScene()
{
	sceneManager =new SceneManager();
}

AppScene::~AppScene()
{
	delete sceneManager;
	sceneManager = nullptr;
}

void AppScene::Initialize()
{
	sceneManager->Initialize();

	//セットされたデータに保存したものがあればそれを適応する
	GlobalVariableManager::GetInstance()->SetLoadAllData();
}

void AppScene::Update()
{
	sceneManager->Update();
}

void AppScene::Draw()
{
	sceneManager->Draw();
}
