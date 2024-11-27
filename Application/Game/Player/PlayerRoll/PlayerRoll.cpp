#include "PlayerRoll.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"
#include"ImGuiManager/ImGuiManager.h"


PlayerRoll::PlayerRoll(Player* player)
{
	//プレイヤーポインタ取得
	player_ = player;

	//入力のインスタンス取得
	inp_ = Input::GetInstance();
	//カメラのインスタンス取得
	camera_ = Camera::GetInstance();

	//ツリーにデバッグ値を追加
	tree_.name_ = "roll";
	tree_.SetValue("速度", &data_.spd);
	tree_.SetValue("ロール時間", &data_.rollSec);
	tree_.SetValue("完全停止までの時間/s", &data_.stopSec);
	tree_.SetMonitorValue("停止時のカウント", &data_.currentStop);
}

void PlayerRoll::Initialize()
{
	//入力方向を取得
	Vector3 move = inp_->GetWASD().SetNormalize();
	//入力のない場合
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
	//正規化する
	move.SetNormalize();

	//正規化して指定した速度量にする
	move *= data_.spd;

	//移動速度
	player_->data_.velo_ = move;

	//移動開始地点を取得
	data_.currentRoll = 0;
	data_.currentStop = 0;

	//アニメーション変更
	player_->SetAnimation(player_->animeName_[(int)Player::AnimationData::Roll], 0.1f, 1.0f, false);

	//回転処理分のスタミナを消費
	player_->DecreaseStamina4Roll();
}

void PlayerRoll::Update()
{
	//カウント処理
	data_.currentRoll += (float)DeltaTimer::deltaTime_;

	//もし指定量以上移動したら減速
	if (data_.currentRoll >= data_.rollSec) {


		//カウント量で段々速度低下
		Vector3 ve = Lerp(player_->data_.velo_, { 0,0,0 }, data_.currentStop);
		player_->data_.velo_ = ve;

		//カウント
		data_.currentStop += (float)DeltaTimer::deltaTime_;
		//カウントMAXで歩行状態へ移行
		if (data_.currentStop >= data_.stopSec) {
			//待機状態へ移行
			player_->SetBehaviorReq(Player::Behavior::IDLE);
			//速度ベクトルを0に
			player_->data_.velo_.SetZero();
		}
		else {
			//止め度合いチェック
			float t = data_.currentStop / data_.stopSec;
			//アニメーション
			player_->SetAnimeTime(true, t);
			//同アニメーションな場合無視されるので何度でも大丈夫＃関数修正予定
			player_->SetAnimation(player_->animeName_[Player::RollEnd], 0, 1, false);

			//少しだけ移動可能
			player_->Move(false, 0.8f);
		}
	}
	else {
		//アニメーション進行
		float t = data_.currentRoll / data_.rollSec;
		player_->SetAnimeTime(true,t);
	}
}

