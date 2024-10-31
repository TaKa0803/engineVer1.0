#include "AppScene.h"

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
}

void AppScene::Update()
{
	sceneManager->Update();
}

void AppScene::Draw()
{
	sceneManager->Draw();
}
