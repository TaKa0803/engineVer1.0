#include "BossUI.h"
#include"Game/Boss/Boss.h"
#include"TextureManager/TextureManager.h"

BossUI::BossUI(Boss* boss)
{
	//ボスのポインタ取得
	boss_=boss;

	//HPバー画像生成
	hpBar_.reset(Sprite::Create(TextureManager::white_, { 32,32 }, { 32,32 },{1,1},{640,360},{0.0f,0.5f}));

	//デバッグ用にパラメータをツリーに格納
	tree_.name_ = "ボスUI";
	tree_.SetTreeData(hpBar_->GetTree("HPバー"));
	tree_.SetValue("barSize", &maxScale_);
}

void BossUI::Initialize()
{
	//画像の最大サイズXを保存
	maxScale_ = hpBar_->GetScale().x;
}


void BossUI::Update()
{
	//HPの比率計算
	float diff = (float)boss_->nowHP/(float)boss_->maxHP_;

	//比率に合わせて画像スケール変更
	float scaleX = Lerp(0, maxScale_, diff);

	//サイズ再設定
	Vector3 s =  hpBar_->GetScale();
	s.x = scaleX;
	hpBar_->SetScale(s);

}

void BossUI::Draw()
{
	//HPbar画像描画
	hpBar_->Draw();
}
