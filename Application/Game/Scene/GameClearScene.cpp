#include "GameClearScene.h"

GameClearScene::GameClearScene()
{
	inp_ = Input::GetInstance();
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Initialize()
{
}

void GameClearScene::Update()
{
	if (inp_->TriggerKey(DIK_SPACE) || inp_->IsTriggerButton(kButtonB)) {
		sceneNo = TITLE;
	}
}

void GameClearScene::Draw()
{
}
