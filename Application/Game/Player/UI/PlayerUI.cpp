#include "PlayerUI.h"
#include"TextureManager/TextureManager.h"
#include"Game/Player/Player.h"

PlayerUI::PlayerUI(Player*player)
{
	player_ = player;

	//HPバー画像生成
	hpBar_.reset(Sprite::Create(TextureManager::white_, { 1,1 }, { 1,1 }, { 1,1 }, { 0,0 }, { 0.0f,0.5f }));

	//ツリーデータ設定
	tree_.SetValue("HPバー最大サイズ", &maxBarScale_);
	tree_.SetTreeData(hpBar_->GetTree("HPバー"));

}

void PlayerUI::Update()
{
	//データ取得
	Player::PlayerData& data = player_->data_;

	//現HPとの割合を取得
	float t = data.currentHP / data.nowHP;
	//サイズXを求める
	float scaleX = Lerp(maxBarScale_, 0, t);
	//現在のサイズを他サイズ含めて参照
	Vector3 s = hpBar_->GetScale();
	//Xのサイズ変更
	s.x = scaleX;
	//変更を反映
	hpBar_->SetScale(s);
}

void PlayerUI::DrawUI()
{
	//HPバー描画
	hpBar_->Draw();
}
