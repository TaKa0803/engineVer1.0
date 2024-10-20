#include"ALPlayer.h"

#include"AudioManager/AudioManager.h"
#include"TextureManager/TextureManager.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AL/Boss/Boss.h"
#include<imgui.h>
#include<json.hpp>
#include<cassert>
#include<fstream>
#include<numbers>

void ALPlayer::StaminaUpdate()
{
	StaminaData& data = data_.stamina;
	data.currentCharge += (float)DeltaTimer::deltaTime_;

	//待機カウント以上で回復処理
	if (data.currentCharge >= data.reChargeSec) {
		data.currentCharge = data.reChargeSec;

		//回復処理
		data.currentStamina += data.healSec * (float)DeltaTimer::deltaTime_;
		//オーバーヒール処理
		if (data.currentStamina>data.maxStamina) {
			data.currentStamina = data.maxStamina;
		}

	}
}

ALPlayer::ALPlayer() {
	//一回しかしない初期化情報
	input_ = Input::GetInstance();

	input_->SetDeadLine(0.3f);

	peM_ = std::make_unique<EffectMove>();


	collider_ = std::make_unique<SphereCollider>();
	collider_->Initialize("player", world_);
	collider_->SetRadius(1.5f);
	collider_->SetTranslate({ 0,1.4f,0 });

	atkCollider_ = std::make_unique<SphereCollider>();
	atkCollider_->Initialize("player atk", world_);
	atkCollider_->SetRadius(1.5f);
	atkCollider_->SetTranslate({ 0,1.4f,0.5f });

	GameObject::Initialize("human");
	model_->IsEnableTexture(false);
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);
	int Index = 0;
	
	textureData = TextureManager::LoadTex("resources/Models/Object/player.png").texNum;

	shadow_ = std::make_unique<CirccleShadow>(world_);


	rolling_ = std::make_unique<PlayerRoll>(this);
	atkM_ = std::make_unique<PlayerATKManager>(this);


}

ALPlayer::~ALPlayer() {

}

void ALPlayer::Initialize() {
	//中身データ初期化
	world_.Initialize();
	world_.translate_.z = 2;
	world_.UpdateMatrix();

	model_->ChangeAnimation(3,0);

	peM_->Initialize({ 1,1,1,1 });




	//ATKConboCount = 1;
	//ATKAnimationSetup_ = false;

	data_ = PlayerData{};

	
	model_->animationRoopSecond_ = 5.0f;

	collider_->Update();
	atkCollider_->Update();
	atkCollider_->isActive_ = false;

	behaviorReq_ = State::Move;
}

void ALPlayer::GetBoss(const Boss* boss)
{
	boss_ = boss;
}

void ALPlayer::Update() {

#ifdef _DEBUG
	rolling_->Debug();

	ImGui::Begin("Player");
	ImGui::Text("Stamina : %4.1f", data_.stamina.currentStamina);
	ImGui::End();
#endif // _DEBUG


	//状態の初期化処理
	if (behaviorReq_) {
		//状態を変更してリクエストを消す
		behavior_ = behaviorReq_.value();
		behaviorReq_ = std::nullopt;

		//攻撃、ローリング後はアニメーションのフラグが変わるのでここで変更しておく
		SetAnimeTime(false);

		//実際の初期化処理
		(this->*BehaviorInitialize[(int)behavior_])();
	}

	peM_->Update();

	//状態の更新
	(this->*BehaviorUpdate[(int)behavior_])();

	//落下の処理
	data_.addFallSpd_ -= data_.fallSpd_;
	world_.translate_.y += data_.addFallSpd_;
	if (world_.translate_.y < 0) {
		world_.translate_.y = 0;
		data_.addFallSpd_ = 0;
	}

	//移動量ベクトル*デルタタイムを加算
	world_.translate_ += data_.velo_ * (float)DeltaTimer::deltaTime_;

	StaminaUpdate();

	//更新
	world_.UpdateMatrix();
	model_->UpdateAnimation();
	atkCollider_->Update();
	collider_->Update();
	shadow_->Update();

}

void (ALPlayer::* ALPlayer::BehaviorInitialize[])() = {
	&ALPlayer::InitMove,		//移動
	&ALPlayer::InitRolling,		//回避
	&ALPlayer::InitATK,			//攻撃
	&ALPlayer::InitHitAction	//被攻撃
};

void (ALPlayer::* ALPlayer::BehaviorUpdate[])() = {
	&ALPlayer::UpdateMove,			//移動
	&ALPlayer::UpdateRolling,		//回避
	&ALPlayer::UpdateATK,			//攻撃
	&ALPlayer::UpdateHitAction		//被攻撃
};

void ALPlayer::Draw() {

	//各モデル描画
	shadow_->Draw();
	GameObject::Draw();
	
	collider_->Draw();
	atkCollider_->Draw();

}

void ALPlayer::DrawParticle()
{
	peM_->Draw();
}

void ALPlayer::DebugWindow(const char* name) {

	float cScale = collider_->GetRadius();

	ImGui::Begin(name);
	ImGui::DragFloat("spd", &data_.spd_, 0.01f);
	ImGui::DragFloat("fall spd", &data_.fallSpd_, 0.01f);

	world_.DrawDebug(name);
	collider_->Debug(name);
	model_->DebugParameter(name);

	ImGui::DragFloat("collider scale", &cScale, 0.1f, 1, 10);

	//model_->DebugParameter(name);
	ImGui::End();

	collider_->SetRadius(cScale);
}

void ALPlayer::OnCollision()
{
}

void ALPlayer::OnCollisionBack(const Vector3& backV)
{
	world_.translate_ += backV;
	world_.UpdateMatrix();
	collider_->Update();

	//高さに関する処理が行われた場合落下速度を初期化
	if (backV.y != 0) {
		data_.addFallSpd_ = 0;
	}
}

Vector3 ALPlayer::SetInputDirection(bool&isZero)
{
	//入力方向をチェック
	Vector3 move = input_->GetWASD().SetNormalize();

	//ローラーがあるならその値を追加
	if (input_->IsControllerActive()) {
		move += input_->GetjoyStickLV3();
	}

	//入力が行われていない場合
	if (move == Vector3{ 0,0,0 }) {

		//すでに向いている方向にする
		move = TransformNormal({ 0,0,-1 }, world_.matWorld_);
		//高さをなくす
		move.y = 0;
		//正規化
		move.SetNormalize();
		//入力なしと見る
		isZero = false;

		//返却
		return move;
	}
	else {
		//カメラから見て違和感のない方向に修正
		move = camera_->SetDirection4Camera(move);
		isZero = true;
	}

	//高さは使わない
	move.y = 0;
	move.SetNormalize();

	//向きを変更する処理
	world_.rotate_.y = GetYRotate({ move.x,move.z }) + ((float)std::numbers::pi);
	

	return move;
}

const Vector3 ALPlayer::GetP2BossVelo()
{
	return boss_->world_.GetWorldTranslate() - world_.GetWorldTranslate();
}

bool ALPlayer::GetATKInput()
{
	int isATK = 0;

	isATK += (int)input_->TriggerKey(DIK_Z);

	//コントローラーがあるときの処理
	if (input_->IsControllerActive()) {
		isATK += (int)input_->IsTriggerButton(kButtonB);
	}

	if (isATK > 1) {
		isATK = 1;
	}

	return (bool)isATK;
}

bool ALPlayer::GetRollInput()
{

	int ans = input_->TriggerKey(DIK_SPACE);

	if (input_->IsControllerActive()) {
		ans += input_->IsPushButton(kButtonA);
	}

	return ans;
}

bool ALPlayer::GetDashInput()
{
	int ans = input_->PushKey(DIK_LSHIFT);
	if (input_->IsControllerActive()) {
		ans += input_->IsPushButton(kButtonX);
	}
	return ans;
}

void ALPlayer::Move() {

	Vector3 move{};
	//ダッシュチェック
	bool isDash = false;

	//移動入力の有無
	bool isMoveInput = false;

	//入力受付と向きなおし処理＆向いた向きベクトル取得
	move = SetInputDirection(isMoveInput);

	//移動量分加算
	move *= data_.spd_;

	//ダッシュキー併用で速度アップ
	if (isMoveInput && GetDashInput() && data_.stamina.currentStamina >= data_.stamina.dashCostSec * (float)DeltaTimer::deltaTime_) {

		isDash = true;

		move *= data_.dashMultiply;

		//スタミナを消費
		data_.stamina.currentStamina -= data_.stamina.dashCostSec * (float)DeltaTimer::deltaTime_;
		data_.stamina.currentCharge = 0;
	}
	
	//移動入力があった場合
	if (isMoveInput) {

		//歩行エフェクト出現
		peM_->SpawnE(world_.GetWorldTranslate());

		//加算
		world_.translate_ += move*(float)DeltaTimer::deltaTime_;
	}


	ModelRoop(isMoveInput,isDash);
}

void ALPlayer::ModelRoop(bool ismove,bool isDash)
{
	if (!ismove) {
		//待機状態になる
		SetAnimation(Idle, 0.1f, 1.0f);
	}
	else {
		//移動状態
		if (!isDash) {
			//歩き
			SetAnimation(Run, 0.1f, 1.0f);
		}
		else {
			//ダッシュ
			SetAnimation(Dash,	0.1f, 1.0f);
		}
	}

}

void ALPlayer::SetAnimation(int animeNum, float count, float loopSec, bool isLoop)
{
	model_->ChangeAnimation(animeNum, count);
	model_->SetAnimationRoop(isLoop);
	model_->animationRoopSecond_ = loopSec;
}







#pragma region 各状態初期化処理


void ALPlayer::InitMove() {

	SetAnimation(3, 0.1f, 1.0f);
}

void ALPlayer::InitRolling() {

	rolling_->Initialize();
	data_.stamina.currentStamina -= data_.stamina.rollCost;
	data_.stamina.currentCharge = 0;
}

void ALPlayer::InitATK() {
	data_.stamina.currentStamina -= data_.stamina.atkCost;
	data_.stamina.currentCharge = 0;

	atkM_->Initialize();

	atkCollider_->isActive_ = true;
}

void ALPlayer::InitHitAction() {

}



#pragma endregion

#pragma region 各状態更新処理

void ALPlayer::UpdateMove() {
	//移動処理
	Move();

	//攻撃状態,回転処理に移るかの処理
	bool isATK, isRoll = { false };

	//各入力取得
	isATK = GetATKInput();
	isRoll = GetRollInput();

	if (isATK&&data_.stamina.currentStamina>=data_.stamina.atkCost) {
		behaviorReq_ = State::ATK;
	}

	if (isRoll&&data_.stamina.currentStamina >= data_.stamina.rollCost) {
		behaviorReq_ = State::Rolling;
	}
	
}

void ALPlayer::UpdateRolling()
{
	rolling_->Update();
}



void ALPlayer::UpdateATK() {

	//攻撃の処理
	atkM_->Update();

	//攻撃処理が終わったフラグを取得
	if (atkM_->isEnd_) {
		behaviorReq_ = State::Move;
	}
}

void ALPlayer::UpdateHitAction() {

}

#pragma endregion