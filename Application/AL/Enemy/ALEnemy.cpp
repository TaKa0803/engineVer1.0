#include"AL/Enemy/ALEnemy.h"

#include"RandomNum/RandomNum.h"
#include"TextureManager/TextureManager.h"
#include"AudioManager/AudioManager.h"
#include"AL/BrokenBody/BrokenBody.h"

#include<numbers>

void ALEnemy::Initialize(const Vector3& position, const EulerWorldTransform* playerWorld) {
	InstancingGameObject::Initialize("PlayerM4");

	IMM_->SetTexture(a3tag_, TextureManager::white_);
	IMM_->SetAnimeNum(a3tag_, 3);
	IMM_->SetTexture(a4tag_, TextureManager::white_);
	IMM_->SetAnimeNum(a4tag_, 4);

	IMM_->SetAnimationRoopFrame(a3tag_,  1,true);
	IMM_->SetAnimationRoopFrame(a4tag_,  5,true);

	color_ = { 1.0f,0.0f,0.0f,1.0f };

	//model_->IsEnableTexture(false);
	//model_->SetBlendMode(BlendMode::kNone);
	world_.translate_ = position;

	tHeight = world_.translate_.y;

	playerWorld_ = playerWorld;
	//idou
	moveSPD_ = RandomNumber::Get(minSPD_, maxSPD_);

	world_.rotate_.y = RandomNumber::Get(0, 3.14f);

	shadow = std::make_unique<InstancingGameObject>();
	shadow->Initialize("DZone");

	shadow->SetColor({ 0,0,0,1 });
	shadow->SetTranslate({ world_.translate_.x,0,world_.translate_.z });
	shadow->SetScale(1.5f);	

	collider_ = std::make_unique<SphereCollider>();
	collider_->Initialize("ene", world_);
	collider_->SetRadius(1.5f);
	collider_->SetTranslate({ 0,1.5f,0 });


	breakSound_ = AudioManager::LoadSoundNum("break");
}

void (ALEnemy::* ALEnemy::BehaviorInitialize[])() = {
	&ALEnemy::StayInitialize,
	&ALEnemy::FollowInitialize,
	&ALEnemy::HitInitialize
};

void (ALEnemy::* ALEnemy::BehaviorUpdate[])() = {
	&ALEnemy::StayUpdate,
	&ALEnemy::FollowUpdate,
	&ALEnemy::HitUpdate
};

void ALEnemy::Update() {

	isHit_ = false;

	FallUpdate();

	//状態の初期化処理
	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();
		behaviorRequest_ = std::nullopt;

		//実際の初期化処理
		(this->*BehaviorInitialize[(int)behavior_])();
	}

	//状態の更新
	(this->*BehaviorUpdate[(int)behavior_])();

	//InstancingGameObject::Update();
	world_.UpdateMatrix();

	collider_->Update();

	shadow->SetTranslate({ world_.translate_.x,0.1f,world_.translate_.z });
	shadow->Update();
}



bool ALEnemy::Collision(SphereCollider* collider) {

	Vector3 backVec;
	if (collider_->IsCollision(collider, backVec)) {

		behavior_ = Hit;

		velocity_ = hitVelo;

		Vector3 myPos = world_.GetWorldTranslate();
		Vector3 pPos = playerWorld_->GetWorldTranslate();

		myPos.y = 0;
		pPos.y = 0;

		Vector3 direc = myPos - pPos;
		direc.SetNormalize();
		direc *= hitSPD_;

		velocity_.x = direc.x;
		velocity_.z = direc.z;

		HP_--;

		behaviorRequest_ = Hit;

		return true;
	}

	return false;
}

Vector3 ALEnemy::OshiDashi(SphereCollider* collider)
{
	isHit_ = true;

	Vector3 backVec;
	if (collider_->IsCollision(collider, backVec)) {
		backVec.y = 0;
		backVec /= 2;
		world_.translate_ += backVec;

		if (backVec.y != 0) {
			addFallspd_ = 0;
		}
	}

	return backVec;
}

void ALEnemy::PushBack(const Vector3& backV)
{
	world_.translate_ += backV;
	world_.UpdateMatrix();

	if (backV.y != 0) {
		addFallspd_ = 0;
	}
}

void ALEnemy::Draw() {

	world_.UpdateMatrix();

	if (behaviorRequest_ == Stay || behavior_ == Stay) {
		//タグに対応したモデルにワールド追加
		IMM_->SetData(a3tag_, world_, 3, color_);
	}
	else{
		//タグに対応したモデルにワールド追加
		IMM_->SetData(a4tag_, world_, 3, color_);
	}
	shadow->Draw();
	//collider_->Draw();
}

#pragma region 各状態の初期化と更新

void ALEnemy::StayInitialize()
{
	velocity_.SetZero();
	animeNum_ = 3;
}

void ALEnemy::FollowInitialize()
{
	animeNum_ = 4;

}

void ALEnemy::HitInitialize()
{
}

void ALEnemy::StayUpdate()
{
	//pk
	Vector3 p_eVelo = playerWorld_->GetWorldTranslate() - world_.GetWorldTranslate();
	//高さを考慮しない
	p_eVelo.y = 0;

	float p_eLength = p_eVelo.GetLength();

	//プレイヤーが追従範囲内の時
	if (p_eLength > stopRange_ && p_eLength < serchRange_) {
		behaviorRequest_ = Follow;

	}
}

void ALEnemy::FollowUpdate()
{
	//プレイヤー方向の向き
	Vector3 p_eVelo = playerWorld_->GetWorldTranslate() - world_.GetWorldTranslate();
	//高さを考慮しない
	p_eVelo.y = 0;

	float p_eLength = p_eVelo.GetLength();

	//プレイヤーが追従範囲内の時
	if (p_eLength > stopRange_ && p_eLength < serchRange_) {

		//プレイヤーの方向に移動
		Vector3 moveVelo{};
		moveVelo = p_eVelo;
		//ノーマライズ
		moveVelo.SetNormalize();
		//移動領分書ける
		moveVelo *= moveSPD_ / maxSPDFrame;


		//速度に追加
		velocity_ += moveVelo;
		//速度ベクトルの量を取得
		float veloSPD = velocity_.GetLength();
		//プレイヤーへの向きベクトルに書ける
		velocity_ = p_eVelo.SetNormalize() * veloSPD;

		
		//最大速度に達していたら移動量もどす
		float spd = Length(velocity_);
		if (spd > moveSPD_) {
			velocity_.SetNormalize();
			velocity_ *= moveSPD_;
		}

		//加算処理
		world_.translate_ += velocity_;

		//向きの処理
		if (moveVelo != Vector3(0, 0, 0)) {
			world_.rotate_.y = GetYRotate({ moveVelo.x,moveVelo.z })+(float)std::numbers::pi;
		}
	}
	else {
		behaviorRequest_ = Stay;

	}
}

void ALEnemy::HitUpdate()
{
	velocity_ += acce;

	world_.translate_ += velocity_;

	world_.rotate_.x -= 0.5f;

	//高さがゼロ以下
	if (world_.translate_.y <= tHeight) {

		//HPがあるなら復帰
		if (HP_ > 0) {
			world_.translate_.y = tHeight;
			world_.rotate_.x = 0;
			behavior_ = Stay;
			velocity_.SetZero();

		}
		else {
			//死亡処理
			isDead_ = true;
			BrokenBody* BB = BrokenBody::GetInstance();
			BB->EffectOccurred(world_, 10);

			//音発生
			AudioManager::PlaySoundData(breakSound_, 0.2f);
		}

	}
}
#pragma endregion


void ALEnemy::FallUpdate()
{
	addFallspd_ -= fallspd_;
	world_.translate_.y += addFallspd_;

	if (world_.translate_.y < 0) {
		world_.translate_.y = 0;
		addFallspd_ = 0;
	}
}
