#include "GameClearScene.h"
#include"TextureManager/TextureManager.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"
#include"AudioManager/AudioManager.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"

GameClearScene::GameClearScene()
{
	//インプットのインスタンス取得
	inp_ = Input::GetInstance();

	//スクリーンショットスプライト生成
	screanShot_.reset(Sprite::Create(TextureManager::white_, { 1280,720 }, { 1280,720 }, { 1280,720 }));
	//クリアテキスト画像を読み込んでスプライト生成
	clearText_.reset(Sprite::Create(TextureManager::LoadTex("resources/Texture/AL/gameClear.png"), { 1280,720 }, { 1280,720 }, { 1280,720 }));
	//Bボタン画像を読み込んでスプライト生成
	bButton_.reset(Sprite::Create(TextureManager::LoadTex("resources/Texture/AL/B.png"), { 180,90 }, { 90,90 }, { 60,60 }));

	//デバッグ用にパラメータをグループに設定
	std::unique_ptr<GlobalVariableGroup>gvg = std::make_unique<GlobalVariableGroup>("クリアシーン");
	gvg->SetTreeData(clearText_->GetTree("クリアテキスト"));
	gvg->SetTreeData(bButton_->GetTree("Bボタン"));

	//クリアの音
	bgmClear_ = AudioManager::LoadSoundNum("clear");
}

void GameClearScene::Initialize()
{
	//すべての音を止める
	AudioManager::GetInstance()->StopAllSounds();
	//クリア音を流す
	AudioManager::PlaySoundData(bgmClear_);
}

void GameClearScene::Update()
{
	//入力で別シーンへ
	if (inp_->TriggerKey(DIK_SPACE) || inp_->IsTriggerButton(kButtonB)) {
		//音を止める
		AudioManager::GetInstance()->StopAllSounds();
		//タイトルシーンへ
		SetScene(SCENE::TITLE);
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
