#include "GameScene.h"

GameScene::GameScene()
{
	sceneManager =new SceneManager();
}

GameScene::~GameScene()
{
	delete sceneManager;
	sceneManager = nullptr;
}

void GameScene::Initialize()
{
	sceneManager->Initialize();
}

void GameScene::Update()
{
	sceneManager->Update();
}

void GameScene::Draw()
{
	sceneManager->Draw();
}
