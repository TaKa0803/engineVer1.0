#include "GameUI.h"
#include"TextureManager/TextureManager.h" 
#include"GvariGroup/GvariGroup.h"

GameUI::GameUI()
{
	//ボタン画像読み込み
	int texture = TextureManager::LoadTex("resources/Texture/AL/B.png").texNum;
	//スプライト生成
	BButton_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 64,64 }, { 1045, 650 }));

	//攻撃アイコン画像読み込み
	texture = TextureManager::LoadTex("resources/Texture/AL/punch.png").texNum;
	//スプライト生成
	punchSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));
	//ボタン座標初期化
	BButton_->SetPosition({ 1155,635 });


	//デバッグのために値をセット
	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("UI");
	gvg->SetValue("待機アイコン座標", &waitATKpos);
	gvg->SetValue("待機アイコンサイズ", &waitATKscale);

	gvg->SetValue("現在アイコン座標", &ATKpos);
	gvg->SetValue("現在アイコンサイズ", &ATKscale);

}

void GameUI::Initialize()
{
}

void GameUI::Update()
{
	punchSp_->SetTVTranslate({ 0.5f,0 });
	punchSp_->SetPosition(waitATKpos);
	punchSp_->SetScale(waitATKscale);
}

void GameUI::Draw()
{
	//パンチ画像描画
	punchSp_->Draw();
	//Bボタン画像描画
	BButton_->Draw();
}

