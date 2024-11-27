#include "BossUI.h"
#include"TextureManager/TextureManager.h"

BossUI::BossUI(Boss* boss)
{
	hpBar_.reset(Sprite::Create(TextureManager::white_, { 32,32 }, { 32,32 },{1,1},{640,360},{0.0f,0.5f}));


	tree_.name_ = "ボスUI";
	tree_.SetTreeData(hpBar_->GetTree("HPバー"));
	tree_.SetValue("バーの色", &barColor_);
}


void BossUI::Update()
{
	hpBar_->SetMaterialDataColor(barColor_);
}

void BossUI::Draw()
{
	hpBar_->Draw();
}
