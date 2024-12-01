#include"Player.h"

#include"AudioManager/AudioManager.h"
#include"TextureManager/TextureManager.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game/Boss/Boss.h"
#include"GvariGroup/GvariGroup.h"

#include"Game/Player/Stamina/PlayerStamina.h"
#include"Game/Player/Idle/PlayerIdle.h"
#include"Game/Player/Down/PlayerDown.h"

#include<imgui.h>
#include<json.hpp>
#include<cassert>
#include<fstream>
#include<numbers>

Player::Player() {
	//一回しかしない初期化情報
	input_ = Input::GetInstance();
	//デッドライン設定
	input_->SetDeadLine(0.3f);

	//状態の数取得
	behaviors_.resize((size_t)Behavior::kNumStates);

	//各状態生成
	behaviors_[(int)Behavior::IDLE] = std::make_unique<PlayerIdle>(this);
	behaviors_[(int)Behavior::Rolling] = std::make_unique<PlayerRoll>(this);
	behaviors_[(int)Behavior::ATK] = std::make_unique<PlayerATKManager>(this);
	behaviors_[(int)Behavior::HITACTION] = std::make_unique<PlayerDown>(this);

	//パーティクルマネージャの生成
	hitPariticle = std::make_unique<ParticleManager>();
	//パーティクルとして出す画像をセットして初期化
	hitPariticle->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);

	//スタミナマネージャ生成
	stamina_ = std::make_unique<PlayerStamina>();

	//移動エフェクト生成
	moveE_ = std::make_unique<EffectMove>();

	//体コライダー生成
	bodyCollider_ = std::make_unique<SphereCollider>();
	//初期化
	bodyCollider_->Initialize("player", world_);

	//攻撃コライダー生成
	atkCollider_ = std::make_unique<SphereCollider>();
	//初期化
	atkCollider_->Initialize("player atk", world_);

	//モデル生成
	GameObject::Initialize("human");
	//画像フラグOFF
	model_->IsEnableTexture(false);
	//アニメーションを有効に
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);

	//円影生成
	shadow_ = std::make_unique<CirccleShadow>(world_);

	//Gvariの値設定
	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("Player");

#ifdef _DEBUG
	gvg->SetMonitorValue("デバッグ用ヒットフラグ", &debugIsHit_);
#endif // _DEBUG
	gvg->SetValue("体力", &data_.HP_);
	gvg->SetValue("移動速度", &data_.spd_);
	gvg->SetValue("ダッシュ時の速度倍率", &data_.dashMultiply);
	gvg->SetValue("無敵時間", &data_.noHitTime_);

	GVariTree anime = GVariTree("アニメーション速度");
	anime.SetValue("待機", &idleAnimeMulti_);
	anime.SetValue("歩き", &moveAnimeMulti_);
	anime.SetValue("走り", &runAnimeMulti_);
	gvg->SetTreeData(anime);



	//ツリーセット
	gvg->SetTreeData(stamina_->GetTree());
	gvg->SetTreeData(behaviors_[(int)Behavior::Rolling]->GetTree());
	gvg->SetTreeData(behaviors_[(int)Behavior::HITACTION]->GetTree());
	gvg->SetTreeData(behaviors_[(int)Behavior::ATK]->GetTree());

	gvg->SetTreeData(hitPariticle->GetTreeData("攻撃ヒット時エフェクト"));
	gvg->SetTreeData(model_->SetDebugParam("model"));
	gvg->SetTreeData(bodyCollider_->GetDebugTree("体コライダー"));
	gvg->SetTreeData(atkCollider_->GetDebugTree("攻撃コライダー"));
}

void Player::Initialize() {
	//中身データ初期化
	world_.Initialize();
	world_.translate_.z = 2;
	world_.UpdateMatrix();

	SetAnimation(animeName_[AnimationData::Idle], animeBlend_, idleAnimeMulti_);

	moveE_->Initialize({ 1,1,1,1 });

	data_ = PlayerData{};

	bodyCollider_->Update();
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

		GlobalInitialize();


		//実際の初期化処理
		behaviors_[(int)behavior_]->Initialize();
	}

	//状態の更新
	behaviors_[(int)behavior_]->Update();



	GlobalUpdate();

}



void Player::Draw() {

	//各モデル描画
	shadow_->Draw();
	GameObject::Draw();

	bodyCollider_->Draw();
	atkCollider_->Draw();

}

void Player::DrawParticle()
{
	//パーティクルを描画
	hitPariticle->Draw();
	moveE_->Draw();
}

void Player::OnCollision()
{
#ifdef _DEBUG
	//デバッグヒットフラグOFFの場合終了
	if (!debugIsHit_) {
		return;
	}
#endif // _DEBUG

	//ヒットフラグONの場合
	if (isHit_) {
		//ヒットフラグをOFF
		isHit_ = false;
		//HPを減らす
		data_.HP_--;
		//状態をヒット状態へ
		behaviorReq_ = Behavior::HITACTION;
		//無敵時間を設定
		data_.currentHitCount_ = data_.noHitTime_;
		//プレイヤーの攻撃コライダーをOFF
		atkCollider_->isActive_ = false;

	}
}

void Player::OnCollisionATKHit()
{
	//攻撃コライダー
	GetATKCollider()->isActive_ = false;
	//コライダー位置からエフェクト出現
	hitPariticle->SpawnE(bodyCollider_->GetWorld().GetWorldTranslate());
}

void Player::OnCollisionBack(const Vector3& backV)
{
	//押し出し量分戻す
	world_.translate_ -= backV;
	//埋まらせないよう調整
	world_.translate_.y = 0;
	//行列更新
	world_.UpdateMatrix();
	//コライダー更新
	bodyCollider_->Update();

}

Vector3 Player::SetInputDirection(bool& isZero)
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
	//プレイヤーからボスへの向きベクトルを返却
	return boss_->world_.GetWorldTranslate() - world_.GetWorldTranslate();
}

bool Player::GetATKInput()
{
	//キー入力でおこなわれていたか取得
	int isATK = (int)input_->TriggerKey(DIK_Z);

	//コントローラーがあるときの処理
	if (input_->IsControllerActive()) {
		isATK += (int)input_->IsTriggerButton(kButtonB);
	}

	//１以上はどちらか有効なので１に
	if (isATK > 1) {
		isATK = 1;
	}

	//boolにして返却
	return (bool)isATK;
}

bool Player::GetRollInput()
{
	//キー入力を取得
	int ans = input_->TriggerKey(DIK_SPACE);
	//コントローラーでの入力を取得
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

	//入力された移動ベクトル
	Vector3 move{};
	//ダッシュ,移動入力の有無
	bool isDash, isMoveInput = { false };

	//入力受付と向きなおし処理＆向いた向きベクトル取得
	move = SetInputDirection(isMoveInput);

	//移動量分加算
	move *= data_.spd_;

	//ダッシュキー併用で速度アップ
	if (canDash && isMoveInput && GetDashInput() && stamina_->CheckStamina(PlayerStamina::Type::DASH)) {
		//スタミナを消費
		stamina_->UseStamina(PlayerStamina::Type::DASH);
		//ダッシュフラグをONに
		isDash = true;
		//速度を増やす
		move *= data_.dashMultiply;
	}

	//移動入力があった場合
	if (isMoveInput) {

		//歩行エフェクト出現
		moveE_->SpawnE(world_.GetWorldTranslate());

		//加算
		world_.translate_ += move * spdMulti * (float)DeltaTimer::deltaTime_;
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

	if (isATK && stamina_->CheckStamina(PlayerStamina::Type::ATK)) {
		behaviorReq_ = Behavior::ATK;
	}

	if (isRoll && stamina_->CheckStamina(PlayerStamina::Type::ROLL)) {
		behaviorReq_ = Behavior::Rolling;
	}
}

void Player::DecreaseStamina4Roll()
{
	//スタミナを消費
	stamina_->UseStamina(PlayerStamina::Type::ROLL);
}

void Player::ModelRoop(bool ismove, bool isDash)
{
	if (!ismove) {
		//待機状態になる
		SetAnimation(animeName_[Idle], animeBlend_, idleAnimeMulti_);
	}
	else {
		//移動状態
		if (!isDash) {
			//歩き
			SetAnimation(animeName_[Moving], animeBlend_, moveAnimeMulti_);
		}
		else {
			//ダッシュ
			SetAnimation(animeName_[Dash], animeBlend_, runAnimeMulti_);
		}
	}

}

void Player::SetAnimation(const std::string& animeName, float count, float loopSec, bool isLoop)
{
	//アニメーションを変更
	model_->ChangeAnimation(animeName, count);
	//ループフラグを変更
	model_->SetAnimationRoop(isLoop);
	//アニメーション速度倍率変更
	model_->animationRoopSecond_ = loopSec;
}


void Player::InitATK() {
	//スタミナを消費
	stamina_->UseStamina(PlayerStamina::Type::ATK);
}

void Player::GlobalInitialize()
{
	//攻撃、ローリング後はアニメーションのフラグが変わるのでここで変更しておく
	//アニメーション進行をアプリ側で処理するフラグをOFF
	SetAnimeTime(false);
	//攻撃コライダー無効化
	atkCollider_->isActive_ = false;
}

void Player::GlobalUpdate()
{

	//移動エフェクト更新
	moveE_->Update();

	//パーティクル更新
	hitPariticle->Update();

	//移動量ベクトル*デルタタイムを加算
	world_.translate_ += data_.velo_ * (float)DeltaTimer::deltaTime_;

	//スタミナの更新
	stamina_->Update();

	//無敵時間の解除処理
	if (!isHit_) {
		if ((data_.currentHitCount_ -= (float)DeltaTimer::deltaTime_) <= 0) {
			isHit_ = true;
		}
	}

	//行列更新
	world_.UpdateMatrix();
	//攻撃コライダー更新
	atkCollider_->Update();
	//体コライダー更新
	bodyCollider_->Update();
	//丸影更新
	shadow_->Update();
}


