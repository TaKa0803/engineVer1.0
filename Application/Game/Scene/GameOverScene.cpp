#include "GameOverScene.h"
#include"TextureManager/TextureManager.h"

GameOverScene::GameOverScene()
{

	inp_ = Input::GetInstance();

	int tex = TextureManager::LoadTex("resources/Texture/AL/gameover.png").texNum;
	logo_.reset(Sprite::Create(tex, { 1280,720 }, { 1280,720 }, { 1280,720 }));

}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Initialize()
{

}

void GameOverScene::Update()
{
	if (inp_->TriggerKey(DIK_SPACE) || inp_->IsTriggerButton(kPadButton::kButtonB)) {
		sceneNo = ALTITLE;
	}
}

void GameOverScene::Draw()
{
	logo_->Draw();
}
