#include "PlayerDash.h"
#include"AL/Player/ALPlayer.h"
#include"DeltaTimer/DeltaTimer.h"
#include"ImGuiManager/ImGuiManager.h"


PlayerDash::PlayerDash(ALPlayer* player)
{
	//プレイヤー取得
	player_ = player;

	inp_ = Input::GetInstance();
	camera_ = Camera::GetInstance();
}

void PlayerDash::Initialize()
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
	data_.stPos = player_->world_.GetMatWorldTranslate();	
	data_.currentStop = 0;
}

void PlayerDash::Update()
{

	//もし指定量以上移動したら減速
	if (Vector3{ data_.stPos - player_->world_.GetMatWorldTranslate() }.GetLength() >= data_.length) {

		//カウント量で段々速度低下
		Vector3 ve = Lerp(player_->data_.velo_, { 0,0,0 }, data_.currentStop);

		player_->data_.velo_ = ve;

		//カウント
		data_.currentStop += (float)DeltaTimer::deltaTime_;
		//カウントMAXで歩行状態へ移行
		if (data_.currentStop >= data_.stopSec) {
			player_->behaviorReq_ = ALPlayer::State::Move;
			player_->data_.velo_.SetZero();
		}
	}
}

void PlayerDash::Debug()
{
#ifdef _DEBUG
	ImGui::Begin("dash");
	ImGui::DragFloat("spd", &data_.spd, 0.01f);
	ImGui::DragFloat("length", &data_.length, 0.01f);
	ImGui::DragFloat("stopSec", &data_.stopSec, 0.01f);
	ImGui::End();
#endif // _DEBUG
}
