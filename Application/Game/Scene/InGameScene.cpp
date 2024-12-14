#include"InGameScene.h"

#include"InstancingModelManager/InstancingModelManager.h"
#include"TextureManager/TextureManager.h"
#include"AudioManager/AudioManager.h"
#include"RandomNum/RandomNum.h"
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"
//#include"MapLoader/MapLoader.h"
#include"PostEffect/PEs/PEHSVFilter.h"
#include"PostEffect/PEs/PEVignetting.h"
#include"ColliderOBB/OBBCollider.h"
#include"Game/Boss/BulletManager/BossBulletManager.h"
#include"GVariableManager/GlobalVaribleManager.h"

InGameScene::InGameScene() {

	//入力のインスタンス取得
	input_ = Input::GetInstance();

	//カメラインスタンス取得
	camera_ = Camera::GetInstance();

	//プレイヤー生成
	player_ = std::make_unique<Player>();
	//カメラのポインタセット
	player_->SetCamera(camera_);

	//ボスのコンストラクタ
	boss_ = std::make_unique<Boss>(player_.get());
	//ボスの攻撃基底クラスにボスのポインタセット
	BossBaseATK::SetBossPointer(boss_.get());

	//プレイヤーにボスのポインタをセット
	player_->GetBoss(boss_.get());

	//追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>(&player_->world_, &boss_->world_);

	//UIの生成
	gameUI_ = std::make_unique<GameUI>();

	//地面の生成
	plane_ = std::make_unique<Plane>();

	//シーン遷移の画像読み込み
	int texture = TextureManager::LoadTex(white).texNum;
	//遷移につかう画像の生成
	sceneC_.reset(Sprite::Create(texture, { 1,1 }, { 1,1 }, { 1280,720 }));
	//色をセット
	sceneC_->SetMaterialDataColor({ 0,0,0,1 });

	//各音の読み込み
	//ゲーム中の音
	bgmGame_ = AudioManager::LoadSoundNum("game");
}


void InGameScene::Initialize() {


	//プレイヤー初期化
	player_->Initialize();

	//UI初期化
	gameUI_->Initialize();

	//地面初期化
	plane_->Initialize();

	//ボス初期化
	boss_->Initialize();

	//追従カメラ初期化
	followCamera_->Initialize();

	//登録されているすべてのグローバルな値をセット
	GlobalVariableManager::GetInstance()->SetLoadAllData();

	//シーン遷移のカウントを最大値にセット
	currentSceneXhangeCount_ = maxSceneChangeCount_;

	//シーン遷移したかのフラグをOFF
	isSceneChange_ = false;
	//開幕のシーン遷移したかフラグをOFF
	preSceneChange_ = false;
	//透明度を1にセット
	sceneC_->SetColorAlpha(1);

	//すべての音を止める
	AudioManager::GetInstance()->StopAllSounds();
	//ゲーム内BGMを流す
	AudioManager::PlaySoundData(bgmGame_, 0.08f);

	//微ネットの値を変更
	PEVignetting::materialData_->darkness = 0.3f;

}

void InGameScene::Update() {
	//ポストエフェクトのデバッグウィンドウをだす
	PostEffectManager::GetInstance()->Debug();

	//プレイヤー更新
	player_->Update();

	//ボス更新
	boss_->Update();

	//追従カメラ更新
	followCamera_->Update(isShake_);

	//当たり判定
	Collision();

	//カメラ更新
	camera_->Update();

	//UI更新
	gameUI_->Update();

	//シーン変更処理
	SceneChange();
}

void InGameScene::Draw() {

	//地面
	plane_->Draw();

	//プレイヤー
	player_->Draw();

	//ボス描画
	boss_->Draw();

	//インスタンシングのモデルを全描画
	InstancingModelManager::GetInstance()->DrawAllModel();

	//プレイヤーパーティクルを描画
	player_->DrawParticle();

	//画面にブルーム効果をかける
	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kBloom, true);
	//画面にビネット効果をかける
	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kVinetting, true);

	//グローバルバリアブル指定
	PostEffectManager::GetInstance()->GvariEffectDraw();

	//プレイヤーUI描画
	player_->DrawUI();

	//ボスのUI描画
	boss_->DrawUI();

	//UIの描画
	gameUI_->Draw();

	//黒遷移の描画
	sceneC_->Draw();
}



void InGameScene::Collision() {

	//押し出しベクトル保存先
	Vector3 vec;
	//ボスの体との接触判定
	if (boss_->GetBodyCollider().IsCollision(player_->GetCollider(), vec)) {
		//押し戻し処理
		player_->OnCollisionBack(vec);
	}

	//ボスの体とプレイヤー攻撃との接触判定
	if (boss_->GetBodyCollider().IsCollision(player_->GetATKCollider(), vec)) {
		//ボスのあたった処理
		boss_->OnCollision();
		//プレイヤーの攻撃がヒット時のプレイヤー処理
		player_->OnCollisionATKHit();

	}

	//プレイヤーとボスとの攻撃の接触判定
	if (player_->GetCollider()->IsCollision(&boss_->GetATKCollider(), vec)) {
		//プレイヤー接触判定
		player_->OnCollision();
		//ボスの攻撃コライダー
		boss_->GetATKCollider().isActive_ = false;
		//カメラシェイク処理ON
		isShake_ = true;

	}

	//プレイヤーとボスの弾と判定
	if (BossBulletManager::GetInstance()->CheckCollision(player_->GetCollider())) {
		//プレイヤー接触判定
		player_->OnCollision();
		//カメラシェイク処理ON
		isShake_ = true;
	}
}

void InGameScene::SceneChange() {

	//ボス死亡時の処理
	if (boss_->nowHP<=0&&!isSceneChange_) {

		//シーン変更フラグをON
		isSceneChange_ = true;
		//すべての音を止める
		AudioManager::GetInstance()->StopAllSounds();
		//クリア音を出す
		//AudioManager::PlaySoundData(bgmClear_, 0.08f);		
		//シーンを画像にして保存
		PostEffectManager::GetInstance()->LoadScene2TexFlagActive();
		//透明度を0にセット
		sceneC_->SetColorAlpha(0);
	}
	
	//プレイヤ死亡字の処理
	if (player_->data_.isDead) {
		//シーン変更フラグをON
		isSceneChange_ = true;

		//音をすべてかえる
		AudioManager::GetInstance()->StopAllSounds();
	}

	//ゲーム終了入力時処理
	if (input_->TriggerKey(DIK_ESCAPE)) {
		//終了フラグをON
		leaveGame = true;
	}

	//次シーン進行処理フラグがONの場合
	if (isSceneChange_&&(boss_->isDead_|| player_->data_.isDead) ) {
		//カウントが最大値を上回った場合
		if (currentSceneXhangeCount_++ >= maxSceneChangeCount_) {

			//透明度を1に変更
			sceneC_->SetColorAlpha(1);
			if (player_->data_.isDead) {
				//ゲームシーンに変更
				sceneNo = GAMEOVER;
			}
			else if (boss_->isDead_) {
				//ゲームシーンに変更
				sceneNo = GAMECLEAR;
			}
		}
		else {
			//透明度を計算
			float alpha = float(currentSceneXhangeCount_ / maxSceneChangeCount_);
			//してした透明度にセット
			sceneC_->SetColorAlpha(alpha);
		}
	}
	else {
		//このシーンに来た際の遷移完了フラグがOFFの場合
		if (!preSceneChange_) {
			//０未満時の処理
			if (currentSceneXhangeCount_-- <= 0) {
				//カウントを0に合わせる
				currentSceneXhangeCount_ = 0;
				//透明度を0に設定
				sceneC_->SetColorAlpha(0);
				//シーン到達時遷移終了フラグをON
				preSceneChange_ = true;
			}
			else {
				//割合から０～１の値を計算
				float alpha = float(currentSceneXhangeCount_ / maxSceneChangeCount_);
				//透明度としてセット
				sceneC_->SetColorAlpha(alpha);
			}
		}
	}
}





