#include "GameClearScene.h"
#include"TextureManager/TextureManager.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"

GameClearScene::GameClearScene()
{
	inp_ = Input::GetInstance();

	screanShot_.reset(Sprite::Create(TextureManager::white_, { 1280,720 }, { 1280,720 },{1280,720}));
	clearText_.reset(Sprite::Create(TextureManager::LoadTex("resources/Texture/AL/gameClear.png").texNum, { 1280,720 }, { 1280,720 }, { 1280,720 }));
	bButton_.reset(Sprite::Create(TextureManager::LoadTex("resources/Texture/AL/B.png").texNum, { 180,90 }, { 90,90 }, { 60,60 }));
	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("クリアシーン");
	gvg->SetTreeData(clearText_->GetTree("クリアテキスト"));
	gvg->SetTreeData(bButton_->GetTree("Bボタン"));
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Initialize()
{
	
}

void GameClearScene::Update()
{
	screanShot_->DrawDebugImGui("sp");

	if (inp_->TriggerKey(DIK_SPACE) || inp_->IsTriggerButton(kButtonB)) {
		sceneNo = TITLE;
	}
}

void GameClearScene::Draw()
{
	//スクショ描画
	screanShot_->Draw(PostEffectManager::GetInstance()->GetSceneTexture());

	//グレイスケール描画
	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kGrayScale, true);

	//クリアテキスト描画
	clearText_->Draw();
	//ボタン画像描画
	bButton_->Draw();
}
