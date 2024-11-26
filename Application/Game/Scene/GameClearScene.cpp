#include "GameClearScene.h"
#include"TextureManager/TextureManager.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"

GameClearScene::GameClearScene()
{
	inp_ = Input::GetInstance();

	logo_.reset(Sprite::Create(TextureManager::white_, { 1280,720 }, { 1280,720 },{1280,720}));
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Initialize()
{
	
}

void GameClearScene::Update()
{
	logo_->DrawDebugImGui("sp");

	if (inp_->TriggerKey(DIK_SPACE) || inp_->IsTriggerButton(kButtonB)) {
		sceneNo = TITLE;
	}
}

void GameClearScene::Draw()
{
	logo_->Draw(PostEffectManager::GetInstance()->GetSceneTexture());
}
