#include "BossMove.h"
#include"AL/ALBoss/ALBoss.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"
#include<numbers>

BossMove::BossMove(ALBoss* boss)
{
	boss_ = boss;
}

void BossMove::Initialize()
{
	data_.currentSec = 0;

	//プレイヤー方向を見続ける
	Vector3 direc = boss_->GetBoss2PlayerDirection();
	if (direc != Vector3(0, 0, 0)) {
		boss_->world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}

	//左右どちらかに進む
	data_.moveV = RandomNumber::Get(-1.0f, 1.0f);
	if (data_.moveV > 0) {
		data_.moveV = 1;
	}
	else {
		data_.moveV = -1;
	}
}

// 2Dベクトル {0,1} を theta ラジアンだけ回転させる関数
Vector2 rotate_vector(float theta, Vector2 velo) {
	// 元のベクトル {0, 1}
	float x = velo.x;
	float y = velo.y;

	Vector2 ans{};

	// 回転行列を使って新しい x, y を計算
	ans.x = x * cosf(theta) - y * sinf(theta);
	ans.y = x * sinf(theta) + y * cosf(theta);

	return ans;
}

void BossMove::Update()
{
	//ここに処理
	//プレイヤー方向から90度傾けた方向に移動
	Vector3 velo = boss_->GetBoss2PlayerDirection();

	//ベクトルを向ける
	Vector2 ans = rotate_vector(data_.moveV *(float)std::numbers::pi / 2.0f, Vector2{ velo.x,velo.z });
	Vector3 v = { ans.x,0,ans.y };


	boss_->world_.translate_ += v.GetNormalizeNum()*data_.spdSec*(float)DeltaTimer::deltaTime_;

	//カウント過ぎれば処理
	data_.currentSec += (float)DeltaTimer::deltaTime_;
	if (data_.currentSec >= data_.moveSec) {
		float rand = RandomNumber::Get(0.0f, 1.0f);

		if (rand < 0.5f) {
			boss_->behaviorReq_ = ALBoss::Behavior::IDLE;
		}
		else {
			boss_->behaviorReq_ = ALBoss::Behavior::ATK;
			//boss_->behaviorReq_ = ALBoss::Behavior::IDLE;
		}
	}
}
