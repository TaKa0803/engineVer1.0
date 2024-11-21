#include "PlayerRoll.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"
#include"ImGuiManager/ImGuiManager.h"


PlayerRoll::PlayerRoll(Player* player)
{
	//プレイヤー取得
	player_ = player;

	inp_ = Input::GetInstance();
	camera_ = Camera::GetInstance();

	tree_.SetValue("速度", &data_.spd);
	tree_.SetValue("距離", &data_.length);
	tree_.SetValue("完全停止までの時間/s", &data_.stopSec);

	tree_.SetMonitorValue("停止時のカウント", &data_.currentStop);
}

void PlayerRoll::Initialize()
{
	//入力方向にダッシュ
	Vector3 move = inp_->GetWASD().SetNormalize();
	if (move == Vector3{ 0,0,0 }) {
		//すでに向いている方向に進む
		move = TransformNormal({0,0,-1}, player_->world_.matWorld_);
		
	}
	else {
		//カメラから見て違和感のない方向に修正
		move = camera_->SetDirection4Camera(move);
	}

	//高さは使わない
	move.y = 0;
	move.SetNormalize();

	//正規化して指定した速度量にする
	move *= data_.spd;

	//移動速度
	player_->data_.velo_ = move;

	//移動開始地点を取得
	data_.stPos = player_->world_.GetWorldTranslate();	
	data_.currentStop = 0;

	player_->SetAnimation(player_->animeName_[(int)Player::AnimationData::Roll], 0.1f, 1.0f, false);
	//player_->SetAnimeTime(true);
}

void PlayerRoll::Update()
{
	//初期位置との距離
	float leng = Vector3{ data_.stPos - player_->world_.GetWorldTranslate() }.GetLength();

	//もし指定量以上移動したら減速
	if (leng >= data_.length) {


		//カウント量で段々速度低下
		Vector3 ve = Lerp(player_->data_.velo_, { 0,0,0 }, data_.currentStop);

		player_->data_.velo_ = ve;

		//カウント
		data_.currentStop += (float)DeltaTimer::deltaTime_;
		//カウントMAXで歩行状態へ移行
		if (data_.currentStop >= data_.stopSec) {
			player_->behaviorReq_ = Player::State::Move;
			player_->data_.velo_.SetZero();
		}
		else {
			float t = data_.currentStop / data_.stopSec;
			player_->SetAnimeTime(true, t);
			player_->SetAnimation(player_->animeName_[Player::RollEnd], 0, 1, false);
		}
	}
	else {
		//アニメーション進行
		float t = leng / data_.length;
		player_->SetAnimeTime(true,t);
	}
}

