#include "BossUI.h"
#include"Game/Boss/Boss.h"
#include"TextureManager/TextureManager.h"

BossUI::BossUI(Boss* boss)
{
	boss_=boss;

	hpBar_.reset(Sprite::Create(TextureManager::white_, { 32,32 }, { 32,32 },{1,1},{640,360},{0.0f,0.5f}));


	tree_.name_ = "ボスUI";
	tree_.SetTreeData(hpBar_->GetTree("HPバー"));
	tree_.SetValue("バーの色", &barColor_);
}

void BossUI::Initialize()
{
	maxScale_ = hpBar_->GetScale().x;
}


void BossUI::Update()
{
	//HPの比率計算
	float diff = (float)boss_->HP_/(float)boss_->maxHP_;

	//比率に合わせて画像スケール変更
	float scaleX = Lerp(0, maxScale_, diff);

	//サイズ再設定
	Vector3 s =  hpBar_->GetScale();
	s.x = scaleX;
	hpBar_->SetScale(s);

	hpBar_->SetMaterialDataColor(barColor_);
}

void BossUI::Draw()
{
	hpBar_->Draw();
}
