#include"Player.h"

#include"AudioManager/AudioManager.h"
#include"TextureManager/TextureManager.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game/Boss/Boss.h"
#include"GvariGroup/GvariGroup.h"

#include"Game/Player/Idle/PlayerIdle.h"
#include"Game/Player/Down/PlayerDown.h"

#include<imgui.h>
#include<json.hpp>
#include<cassert>
#include<fstream>
#include<numbers>

void Player::StaminaUpdate()
{
	StaminaData& data = data_.stamina;
	data.currentCharge += (float)DeltaTimer::deltaTime_;

	//待機カウント以上で回復処理
	if (data.currentCharge >= data.rechargeSec) {
		data.currentCharge = data.rechargeSec;

		//回復処理
		data.currentStamina += data.healSec * (float)DeltaTimer::deltaTime_;
		//オーバーヒール処理
		if (data.currentStamina>data.maxStamina) {
			data.currentStamina = data.maxStamina;
		}

	}
}

Player::Player() {
	//一回しかしない初期化情報
	input_ = Input::GetInstance();
	input_->SetDeadLine(0.3f);

	behaviors_.resize((size_t)Behavior::kNumStates);

	behaviors_[(int)Behavior::IDLE] = std::make_unique<PlayerIdle>(this);
	behaviors_[(int)Behavior::Rolling] = std::make_unique<PlayerRoll>(this);
	behaviors_[(int)Behavior::ATK] = std::make_unique<PlayerATKManager>(this);
	behaviors_[(int)Behavior::HITACTION] = std::make_unique<PlayerDown>(this);


	moveE_ = std::make_unique<EffectMove>();


	collider_ = std::make_unique<SphereCollider>();
	collider_->Initialize("player", world_);
	collider_->SetRadius(1.5f);
	collider_->SetTranslate({ 0,1.4f,0 });

	atkCollider_ = std::make_unique<SphereCollider>();
	atkCollider_->Initialize("player atk", world_);
	atkCollider_->SetRadius(4.0f);
	atkCollider_->SetTranslate({ 0,1.4f,0.5f });

	GameObject::Initialize("human");
	model_->IsEnableTexture(false);
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);
	int Index = 0;
	
	textureData = TextureManager::LoadTex("resources/Models/Object/player.png").texNum;

	shadow_ = std::make_unique<CirccleShadow>(world_);

	//Gvariの値設定
	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("Player");

	gvg->SetMonitorValue("ヒットフラグ", &isHit_);
	gvg->SetValue("体力", &data_.HP_);
	gvg->SetValue("移動速度", &data_.spd_);
	gvg->SetValue("ダッシュ時の速度倍率", &data_.dashMultiply);
	gvg->SetValue("落下速度", &data_.fallSpd_);
	
	//スタミナ関係
	StaminaData& sData = data_.stamina;
	GVariTree staminaTree = GVariTree("スタミナ");
	staminaTree.SetValue("最大スタミナ", &sData.maxStamina);
	staminaTree.SetValue("回復開始までの時間", &sData.rechargeSec);
	staminaTree.SetValue("一秒間の回復量", &sData.healSec);
	staminaTree.SetValue("回転時の消費量", &sData.rollCost);
	staminaTree.SetValue("ダッシュ時の消費量/1s", &sData.dashCostSec);
	staminaTree.SetValue("攻撃時の消費量", &sData.atkCost);
	staminaTree.SetMonitorValue("現スタミナ", &sData.currentStamina);


	//ツリーセット
	gvg->SetTreeData(staminaTree);
	gvg->SetTreeData(behaviors_[(int)Behavior::Rolling]->GetTree());
	gvg->SetTreeData(behaviors_[(int)Behavior::HITACTION]->GetTree());

	gvg->SetTreeData(model_->SetDebugParam("model"));
	gvg->SetTreeData(collider_->GetDebugTree("体コライダー"));
	gvg->SetTreeData(atkCollider_->GetDebugTree("攻撃コライダー"));
}

void Player::Initialize() {
	//中身データ初期化
	world_.Initialize();
	world_.translate_.z = 2;
	world_.UpdateMatrix();

	model_->ChangeAnimation(animeName_[Idle], 0);

	moveE_->Initialize({ 1,1,1,1 });




	//ATKConboCount = 1;
	//ATKAnimationSetup_ = false;

	data_ = PlayerData{};

	
	model_->animationRoopSecond_ = 5.0f;

	collider_->Update();
	atkCollider_->Update();
	atkCollider_->isActive_ = false;

	behaviorReq_ = Behavior::IDLE;
}

void Player::GetBoss(const Boss* boss)
{
	boss_ = boss;
}

void Player::SetCamera(Camera* camera)
{
	camera_ = camera; 
}

void Player::Update() {

	//状態の初期化処理
	if (behaviorReq_) {
		//状態を変更してリクエストを消す
		behavior_ = behaviorReq_.value();
		behaviorReq_ = std::nullopt;

		//攻撃、ローリング後はアニメーションのフラグが変わるのでここで変更しておく
		SetAnimeTime(false);

		//実際の初期化処理
		//(this->*BehaviorInitialize[(int)behavior_])();

		behaviors_[(int)behavior_]->Initialize();
	}

	//状態の更新
	//(this->*BehaviorUpdate[(int)behavior_])();
	behaviors_[(int)behavior_]->Update();

	moveE_->Update();



	//落下の処理
	//data_.addFallSpd_ -= data_.fallSpd_;
	//world_.translate_.y += data_.addFallSpd_;
	//if (world_.translate_.y < 0) {
	//	world_.translate_.y = 0;
	//	data_.addFallSpd_ = 0;
	//}

	//移動量ベクトル*デルタタイムを加算
	world_.translate_ += data_.velo_ * (float)DeltaTimer::deltaTime_;

	StaminaUpdate();

	//無敵時間の解除処理
	if (!isHit_) {
		if ((data_.currentHitCount_ -= (float)DeltaTimer::deltaTime_) >= 0) {
			isHit_ = true;
		}
	}

	//更新
	world_.UpdateMatrix();
	atkCollider_->Update();
	collider_->Update();
	shadow_->Update();

}



void Player::Draw() {

	//各モデル描画
	shadow_->Draw();
	GameObject::Draw();
	
	collider_->Draw();
	atkCollider_->Draw();

}

void Player::DrawParticle()
{
	moveE_->Draw();
}



void Player::OnCollision()
{

	if (isHit_) {
		data_.HP_--;

		behaviorReq_ = Behavior::HITACTION;

		isHit_ = false;
		data_.currentHitCount_ = data_.noHitTime_;
	}
}

void Player::OnCollisionBack(const Vector3& backV)
{

	world_.translate_ -= backV;
	world_.translate_.y = 0;
	world_.UpdateMatrix();
	collider_->Update();

	//高さに関する処理が行われた場合落下速度を初期化
	if (backV.y != 0) {
		data_.addFallSpd_ = 0;
	}
}

Vector3 Player::SetInputDirection(bool&isZero)
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

const Vector3 Player::GetP2BossVelo()
{
	return boss_->world_.GetWorldTranslate() - world_.GetWorldTranslate();
}

bool Player::GetATKInput()
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

bool Player::GetRollInput()
{

	int ans = input_->TriggerKey(DIK_SPACE);

	if (input_->IsControllerActive()) {
		ans += input_->IsPushButton(kButtonA);
	}

	return ans;
}

bool Player::GetDashInput()
{
	int ans = input_->PushKey(DIK_LSHIFT);
	if (input_->IsControllerActive()) {
		ans += input_->IsPushButton(kButtonX);
	}
	return ans;
}

void Player::Move(bool canDash, float spdMulti) {

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
	if (canDash&&isMoveInput && GetDashInput() && data_.stamina.currentStamina >= data_.stamina.dashCostSec * (float)DeltaTimer::deltaTime_) {

		isDash = true;

		move *= data_.dashMultiply;

		//スタミナを消費
		data_.stamina.currentStamina -= data_.stamina.dashCostSec * (float)DeltaTimer::deltaTime_;
		data_.stamina.currentCharge = 0;
	}
	
	//移動入力があった場合
	if (isMoveInput) {

		//歩行エフェクト出現
		moveE_->SpawnE(world_.GetWorldTranslate());

		//加算
		world_.translate_ += move*spdMulti*(float)DeltaTimer::deltaTime_;
	}

	//走れる場合のみアニメーション変更
	if (canDash) {
		ModelRoop(isMoveInput, isDash);
	}
}

void Player::ChangeBehavior()
{
	//攻撃状態,回転処理に移るかの処理
	bool isATK, isRoll = { false };

	//各入力取得
	isATK = GetATKInput();
	isRoll = GetRollInput();

	if (isATK && data_.stamina.currentStamina >= data_.stamina.atkCost) {
		behaviorReq_ = Behavior::ATK;
	}

	if (isRoll && data_.stamina.currentStamina >= data_.stamina.rollCost) {
		behaviorReq_ = Behavior::Rolling;
	}
}

void Player::DecreaseStamina4Roll()
{
	data_.stamina.currentStamina -= data_.stamina.rollCost;
	data_.stamina.currentCharge = 0;
}

void Player::ModelRoop(bool ismove,bool isDash)
{
	if (!ismove) {
		//待機状態になる
		SetAnimation(animeName_[Idle], 0.1f, 1.0f);
	}
	else {
		//移動状態
		if (!isDash) {
			//歩き
			SetAnimation(animeName_[Moving], 0.1f, 1.0f);
		}
		else {
			//ダッシュ
			SetAnimation(animeName_[Dash],	0.1f, 1.0f);
		}
	}

}

void Player::SetAnimation(const std::string&animeName, float count, float loopSec, bool isLoop)
{
	model_->ChangeAnimation(animeName, count);
	model_->SetAnimationRoop(isLoop);
	model_->animationRoopSecond_ = loopSec;
}


void Player::InitATK() {
	data_.stamina.currentStamina -= data_.stamina.atkCost;
	data_.stamina.currentCharge = 0;

	atkCollider_->isActive_ = true;
}


