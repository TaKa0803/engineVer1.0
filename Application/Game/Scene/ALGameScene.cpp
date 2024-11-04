#include"ALGameScene.h"

#include<imgui.h>

#include"InstancingModelManager/InstancingModelManager.h"
#include"TextureManager/TextureManager.h"
#include"AudioManager/AudioManager.h"
#include"RandomNum/RandomNum.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"
#include"MapLoader/MapLoader.h"
#include"PostEffect/PEs/PEHSVFilter.h"
#include"PostEffect/PEs/PEVignetting.h"
#include"ColliderOBB/OBBCollider.h"
#include"Game/Boss/BulletManager/BossBulletManager.h"

ALGameScene::ALGameScene() {
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

	MapLoader::GetInstance()->LoadLevelEditor("untitled", ".json");
	MapLoader::GetInstance()->CreateModel(0);
	std::vector<std::unique_ptr<OBBCollider>>& datas = MapLoader::GetInstance()->GetColliderData();

	for (auto& data : datas) {
		if (data->colliderTag_ == "plane c") {
			data->isActive_ = false;
			break;
		}
	}


	brokenBody_ = BrokenBody::GetInstance();

	int texture = TextureManager::LoadTex(white).texNum;
	sceneC_.reset(Sprite::Create(texture, { 1,1 }, { 1,1 }, { 1280,720 }));
	sceneC_->SetMaterialDataColor({ 0,0,0,1 });


	bgmGame_ = AudioManager::LoadSoundNum("game");
	bgmClear_ = AudioManager::LoadSoundNum("clear");



	peM_ = std::make_unique<ParticleManager>();
	EmiterSphere* emit = peM_->GetEmiterData();
	emit->speed = { 0.1f,1.5f };
}

ALGameScene::~ALGameScene() {
	brokenBody_->Finalize();
}

void ALGameScene::Initialize() {

	scene_ = Game;
	limitMinute = maxmilitMinute;

	player_->Initialize();

	gameUI_->Initialize();

	plane_->Initialize();

	boss_->Init();

	followCamera_->Initialize();







	alphaNum_ = 0;
	nowFrameCountEnemy_ = 0;



	serchComplete_ = false;


	brokenBody_->Initialize();

	sceneXhangeCount_ = maxSceneChangeCount_;
	isSceneChange_ = false;
	preSceneChange_ = false;
	sceneC_->SetColorAlpha(1);

	AudioManager::GetInstance()->StopAllSounds();
	AudioManager::PlaySoundData(bgmGame_, 0.08f);

	PEHSVFilter::materialData_->effective = 1.0f;
	PEHSVFilter::materialData_->saturation = 0.3f;
	PEVignetting::materialData_->darkness = 0.3f;

	peM_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);
	peM_->SetOnlyImpact(true);
	EmiterSphere* emit = peM_->GetEmiterData();

	emit->color = { 0,0,1,1 };
}



void ALGameScene::Update() {

	PostEffectManager::GetInstance()->Debug();

	peM_->Update();

	switch (scene_) {
	case ALGameScene::Game:

#pragma region ゲームシーン
		//デバッグウィンドウ表示
		DebugWindows();

		//マップデータ更新
		MapLoader::GetInstance()->UpdateLevelData();

		//プレイヤー更新
		player_->Update();

		boss_->Update();

		//追従カメラ更新
		followCamera_->Update(isShake_);

		//当たり判定
		Collision();

		//壊れた体の演出
		brokenBody_->Update();

#pragma endregion

		break;
	case ALGameScene::Clear:

		break;
	default:
		break;
	}


	camera_->Update();

	gameUI_->Update();

	SceneChange();




}

void ALGameScene::Draw() {

	//地面
	//MapLoader::GetInstance()->DrawLevelData();

	plane_->Draw();

	//プレイヤー
	player_->Draw();

	//
	boss_->Draw();

	//砕けた体描画
	brokenBody_->Draw();

	//インスタンシングのモデルを全描画
	InstancingModelManager::GetInstance()->DrawAllModel();


	peM_->Draw();
	player_->DrawParticle();

	//PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kVinetting, true);



	int Count = 0;
	switch (scene_) {
	case ALGameScene::Game:
		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kLightOutline, true);

		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kVinetting, true);

		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kHSV, true);

		if (Count >= 200) {
			PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kRadialBlur, true);
		}
		gameUI_->DrawGame();

		break;
	case ALGameScene::Clear:
		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kDepthBasedOutline, true);

		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kGaussianFilter, true);


		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kGrayScale, true);

		gameUI_->DrawClear();
		break;
	default:
		break;
	}


	sceneC_->Draw();
}

void ALGameScene::DebugWindows() {

#ifdef _DEBUG

	//カメラのデバッグ表示
	camera_->DrawDebugWindow("camera");

	//プレイヤーデバッグ表示
	player_->DebugWindow("player");

	plane_->DebagWindow();



#endif // _DEBUG

}

void ALGameScene::Collision() {

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

void ALGameScene::SceneChange() {



	switch (scene_) {
	case (int)ALGameScene::Game:
		if (boss_->HP_ <= 0) {
			scene_ = Clear;
			AudioManager::GetInstance()->StopAllSounds();
			AudioManager::PlaySoundData(bgmClear_, 0.08f);


			//if (scene_ == ALGameScene::Game && boss_->isDead_) {
			//	scene_ = Clear;
			//	AudioManager::GetInstance()->StopAllSounds();
			//	AudioManager::PlaySoundData(bgmClear_, 0.08f);
			//	sceneC_->SetColorAlpha(1);
			//	sceneNo = ALTITLE;
			//}
		}

		if (player_->data_.HP_<=0) {
			sceneNo =GAMEOVER;
			AudioManager::GetInstance()->StopAllSounds();
		}


		break;
	case (int)ALGameScene::Clear:

		if (input_->TriggerKey(DIK_SPACE) || input_->IsTriggerButton(kButtonB)) {
			isSceneChange_ = true;
		}

		break;
	default:
		break;
	}


	if (input_->TriggerKey(DIK_ESCAPE)) {
		leaveGame = true;
	}

	if (isSceneChange_) {

		float alpha = float(sceneXhangeCount_ / maxSceneChangeCount_);

		sceneC_->SetColorAlpha(alpha);

		if (sceneXhangeCount_++ >= maxSceneChangeCount_) {
			sceneC_->SetColorAlpha(1);
			sceneNo = ALTITLE;

		}
	}
	else {

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





