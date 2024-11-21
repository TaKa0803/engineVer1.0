#include"GameScene.h"

#include"InstancingModelManager/InstancingModelManager.h"
#include"TextureManager/TextureManager.h"
#include"AudioManager/AudioManager.h"
#include"RandomNum/RandomNum.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"
//#include"MapLoader/MapLoader.h"
#include"PostEffect/PEs/PEHSVFilter.h"
#include"PostEffect/PEs/PEVignetting.h"
#include"ColliderOBB/OBBCollider.h"
#include"Game/Boss/BulletManager/BossBulletManager.h"
#include"GVariableManager/GVaribleManager.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();

	camera_ = Camera::GetInstance();
	player_ = std::make_unique<Player>();
	player_->SetCamera(camera_);

	boss_ = std::make_unique<Boss>(player_.get());
	IBossATK::SetBossPointer(boss_.get());
	followCamera_ = std::make_unique<FollowCamera>(&player_->GetWorld(), &boss_->GetWorld());


	gameUI_ = std::make_unique<GameUI>();

	plane_ = std::make_unique<Plane>();
	plane_->model_->SetUVScale({ 500, 500 });



	int texture = TextureManager::LoadTex(white).texNum;
	sceneC_.reset(Sprite::Create(texture, { 1,1 }, { 1,1 }, { 1280,720 }));
	sceneC_->SetMaterialDataColor({ 0,0,0,1 });


	bgmGame_ = AudioManager::LoadSoundNum("game");
	bgmClear_ = AudioManager::LoadSoundNum("clear");



	particleM_ = std::make_unique<ParticleManager>();
	EmiterSphere* emit = particleM_->GetEmiterData();
	emit->speed = { 0.1f,1.5f };
}

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	limitMinute = maxmilitMinute;

	player_->Initialize();

	gameUI_->Initialize();

	plane_->Initialize();

	boss_->Init();

	followCamera_->Initialize();

	GlobalVariableManager::GetInstance()->SetLoadAllData();





	alphaNum_ = 0;
	nowFrameCountEnemy_ = 0;



	serchComplete_ = false;



	sceneXhangeCount_ = maxSceneChangeCount_;
	isSceneChange_ = false;
	preSceneChange_ = false;
	sceneC_->SetColorAlpha(1);

	AudioManager::GetInstance()->StopAllSounds();
	AudioManager::PlaySoundData(bgmGame_, 0.08f);

	PEHSVFilter::materialData_->effective = 1.0f;
	PEHSVFilter::materialData_->saturation = 0.3f;
	PEVignetting::materialData_->darkness = 0.3f;

	particleM_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);
	particleM_->SetOnlyImpact(true);
	EmiterSphere* emit = particleM_->GetEmiterData();

	emit->color = { 0,0,1,1 };
}



void GameScene::Update() {

	PostEffectManager::GetInstance()->Debug();

	particleM_->Update();



#pragma region ゲームシーン

	//マップデータ更新
	//MapLoader::GetInstance()->UpdateLevelData();

	//プレイヤー更新
	player_->Update();

	boss_->Update();

	//追従カメラ更新
	followCamera_->Update(isShake_);

	//当たり判定
	Collision();



#pragma endregion




	camera_->Update();

	gameUI_->Update();

	SceneChange();




}

void GameScene::Draw() {

	//地面
	plane_->Draw();

	//プレイヤー
	player_->Draw();

	//
	boss_->Draw();

	//インスタンシングのモデルを全描画
	InstancingModelManager::GetInstance()->DrawAllModel();


	particleM_->Draw();
	player_->DrawParticle();


	

	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kBloom, true);
	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kVinetting, true);

	gameUI_->DrawGame();



}



void GameScene::Collision() {

	//ボスの体との接触判定
	Vector3 vec;
	if (boss_->GetBodyCollider().IsCollision(player_->GetATKCollider(), vec)) {
		boss_->OnCollision();
		player_->GetATKCollider()->isActive_ = false;
	}

	//プレイヤーのボス攻撃の当たり判定
	if (player_->GetCollider()->IsCollision(&boss_->GetATKCollider(), vec)) {
		player_->OnCollision();
		boss_->GetATKCollider().isActive_ = false;

	}

	//プレイヤーとボスの弾野と判定
	if (BossBulletManager::GetInstance()->CheckCollision(player_->GetCollider())) {
		player_->OnCollision();
	}
}

void GameScene::SceneChange() {



	//ボス死亡
	if (boss_->isDead_) {
		isSceneChange_ = true;
		AudioManager::GetInstance()->StopAllSounds();
		AudioManager::PlaySoundData(bgmClear_, 0.08f);		
		sceneC_->SetColorAlpha(1);
	}
	

	//プレイヤ死亡
	if (player_->data_.HP_ <= 0) {
		sceneNo = GAMEOVER;
		AudioManager::GetInstance()->StopAllSounds();
	}




	//ゲーム終了処理
	if (input_->TriggerKey(DIK_ESCAPE)) {
		leaveGame = true;
	}

	//次シーン進行処理
	if (isSceneChange_) {

		float alpha = float(sceneXhangeCount_ / maxSceneChangeCount_);

		sceneC_->SetColorAlpha(alpha);

		if (sceneXhangeCount_++ >= maxSceneChangeCount_) {
			sceneC_->SetColorAlpha(1);
			sceneNo = GAMECLEAR;

		}
	}
	else {

		//シーン到達時処理
		if (!preSceneChange_) {

			float alpha = float(sceneXhangeCount_ / maxSceneChangeCount_);

			sceneC_->SetColorAlpha(alpha);

			if (sceneXhangeCount_-- <= 0) {
				sceneXhangeCount_ = 0;
				sceneC_->SetColorAlpha(0);
				preSceneChange_ = true;
			}

		}

	}

}





