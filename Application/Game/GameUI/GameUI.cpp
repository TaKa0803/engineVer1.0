#include "GameUI.h"
#include"TextureManager/TextureManager.h" 
#include"GlobalVariable/Group/GlobalVariableGroup.h"

using namespace Engine;

GameUI::GameUI()
{
	//ボタン画像読み込み
	int texture = TextureManager::LoadTex("resources/Texture/AL/B.png");
	//スプライト生成
	BButton_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 64,64 }, { 1045, 650 }));

	//ボタン画像読み込み
	texture = TextureManager::LoadTexShortPath("AL/X.png");
	//スプライト生成
	XButton_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 64,64 }, { 1045, 650 }));

	//ボタン画像読み込み
	texture = TextureManager::LoadTexShortPath("AL/A.png");
	//スプライト生成
	AButton_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 64,64 }, { 1045, 650 }));


	//攻撃アイコン画像読み込み
	texture = TextureManager::LoadTexShortPath("AL/punch.png");
	//スプライト生成
	punchSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));


	texture = TextureManager::LoadTexShortPath("AL/dash.png");
	//スプライト生成
	dashSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));
	
	texture = TextureManager::LoadTexShortPath("AL/roll.png");
	//スプライト生成
	rollSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));

	//デバッグのために値をセット
	std::unique_ptr<GlobalVariableGroup>gvg = std::make_unique<GlobalVariableGroup>("UI");
	gvg->SetTreeData(BButton_->GetTree("Bボタン"));
	gvg->SetTreeData(XButton_->GetTree("Xボタン"));
	gvg->SetTreeData(AButton_->GetTree("Aボタン"));

	gvg->SetTreeData(punchSp_->GetTree("punchボタン"));
	gvg->SetTreeData(dashSp_->GetTree("dashボタン"));
	gvg->SetTreeData(rollSp_->GetTree("rollボタン"));

}

void GameUI::Initialize()
{
}

void GameUI::Update()
{

}

void GameUI::Draw()
{
	//操作アイコン画像描画
	punchSp_->Draw();
	dashSp_->Draw();
	rollSp_->Draw();


	//ボタン画像描画
	BButton_->Draw();
	XButton_->Draw();
	AButton_->Draw();

}

