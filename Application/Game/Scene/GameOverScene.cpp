#include "GameOverScene.h"
#include"TextureManager/TextureManager.h"

GameOverScene::GameOverScene()
{
	//入力処理のインスタンス取得
	inp_ = Input::GetInstance();

	//画像を読み込む
	int tex = TextureManager::LoadTex("resources/Texture/AL/gameover.png").texNum;
	//読み込んだ画像でスプライトを生成
	logo_.reset(Sprite::Create(tex, { 1280,720 }, { 1280,720 }, { 1280,720 }));

}

void GameOverScene::Initialize()
{
}

void GameOverScene::Update()
{
	//キー入力で次シーンへ
	if (inp_->TriggerKey(DIK_SPACE) || inp_->IsTriggerButton(kPadButton::kButtonB)) {
		//タイトルへ戻る
		sceneNo = TITLE;
	}
}

void GameOverScene::Draw()
{
	//ロゴの描画
	logo_->Draw();
}
