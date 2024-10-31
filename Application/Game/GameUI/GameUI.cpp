#include "GameUI.h"
#include"TextureManager/TextureManager.h" 

GameUI::GameUI()
{
	int texture = TextureManager::LoadTex("resources/Texture/AL/skill.png").texNum;
	skillSp_.reset(Sprite::Create(texture, { 90,90 }, { 90,90 }, { 90,90 }, { 1000,600 }));



	texture = TextureManager::LoadTex("resources/Texture/AL/B.png").texNum;
	BButton_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 64,64 }, { 1045, 650 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/punch.png").texNum;
	punchSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));

	texture = TextureManager::LoadTex("resources/Texture/AL/kick.png").texNum;
	kickSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/ult.png").texNum;
	ultSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/black.png").texNum;
	backScreen_.reset(Sprite::Create(texture, { 64,64 }, { 64,64 }, { 1280,720 }));

	texture = TextureManager::LoadTex("resources/Texture/AL/result.png").texNum;
	resultText_.reset(Sprite::Create(texture, { 320,90 }, { 320,90 }, { 830,290 }));
}

void GameUI::Initialize()
{
	backScreen_->SetColorAlpha(0);
	resultText_->SetColorAlpha(0);
}

void GameUI::Update()
{
#ifdef _DEBUG
	resultText_->DrawDebugImGui("taosita");

	skillSp_->DrawDebugImGui("skill");
	BButton_->DrawDebugImGui("BButton");
	punchSp_->DrawDebugImGui("punch");
#endif // _DEBUG


	switch (BState_) {
	case GameUI::Wait:
		skillSp_->SetPosition(ATKpos);
		skillSp_->SetScale(ATKscale);

		BButton_->SetPosition({ 1155,635 });

		punchSp_->SetTVTranslate({ 0.5f,0 });
		punchSp_->SetPosition(waitATKpos);
		punchSp_->SetScale(waitATKscale);





		break;
	case GameUI::Punch:

		punchSp_->SetTVTranslate({ 0.0f,0.0f });
		punchSp_->SetPosition(ATKpos);
		punchSp_->SetScale(ATKscale);

		kickSp_->SetTVTranslate({ 0.5f,0.0f });
		kickSp_->SetPosition(waitATKpos);
		kickSp_->SetScale(waitATKscale);


		break;
	case GameUI::Kick:
		kickSp_->SetTVTranslate({ 0.0f,0.0f });
		kickSp_->SetPosition(ATKpos);
		kickSp_->SetScale(ATKscale);

		ultSp_->SetTVTranslate({ 0.5f,0.0f });
		ultSp_->SetPosition(waitATKpos);
		ultSp_->SetScale(waitATKscale);




		break;
	case GameUI::Ult:
		ultSp_->SetTVTranslate({ 0.0f,0.0f });
		ultSp_->SetPosition(ATKpos);
		ultSp_->SetScale(ATKscale);




		break;
	default:
		break;
	}


}

void GameUI::DrawGame()
{
	switch (BState_) {
	case GameUI::Wait:

		skillSp_->Draw();
		punchSp_->Draw();

		break;
	case GameUI::Punch:

		punchSp_->Draw();

		kickSp_->Draw();

		break;
	case GameUI::Kick:

		kickSp_->Draw();

		ultSp_->Draw();


		break;
	case GameUI::Ult:

		ultSp_->Draw();

		break;
	default:
		break;
	}

	BButton_->Draw();

}

void GameUI::DrawClear()
{
		backScreen_->Draw();
		resultText_->Draw();
}

void GameUI::SetBState(int state)
{
	if (state == 0) {
		BState_ = Wait;
	}
	if (state == 1) {
		BState_ = Punch;
	}if (state == 2) {
		BState_ = Kick;
	}
	if (state == 3) {
		BState_ = Ult;
	}
}
