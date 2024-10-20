#include "PlayerPunch.h"
#include"AL/Player/ALPlayer.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AudioManager/AudioManager.h"

PlayerPunch::PlayerPunch(ALPlayer* player)
{
	player_ = player;

	punchSound_ = AudioManager::LoadSoundNum("com1");
	kickSound_ = AudioManager::LoadSoundNum("com2");
	drilSound_ = AudioManager::LoadSoundNum("com3");
}
 
void PlayerPunch::Initialize()
{
	//攻撃の状態取得
	state_ = St;
	isEnd_ = false;

	//各シーンカウント
	parameters_.count = 0;
	//初期化処理
	parameters_.isInit = false;
	//連撃回数
	parameters_.atkCount_ = 0;
	player_->SetAnimeTime(true, 0);

	//向きを切り替える
	bool ans;
	player_->SetInputDirection(ans);
}

void PlayerPunch::Update()
{
	//各状態の値加算値加算
	parameters_.count += (float)DeltaTimer::deltaTime_;

	//攻撃の始動、終了の処理
	CheckNextState();

	//終了状態までいっていない場合
	if (state_ != CheckEnd) {

		//攻撃が一段目の場合
		if (parameters_.atkCount_ == 0) {
			//初期化処理が行われていなければ初期化
			if (!parameters_.isInit) {
				parameters_.isInit = true;
				//アニメーションをセット
				player_->SetAnimation(ALPlayer::PrePunch1, 0.1f, 1, false);

			}
			else {
				//アニメーションの更新速度をこちらで調整
				if (state_ == Atk) {
					player_->SetAnimation(ALPlayer::Punch1, 0.1f, 1, false);
				}
				else if (state_ == Ed) {
					player_->SetAnimation(ALPlayer::Idle, 0.1f, 1, false);
				}
			}
		}
		else if (parameters_.atkCount_ == 1) {
			//初期化処理が行われていなければ初期化
			if (!parameters_.isInit) {
				parameters_.isInit = true;
				//アニメーションをセット
				player_->SetAnimation(ALPlayer::PrePunch2, 0.1f, 1, false);

			}
			else {
				//アニメーションの更新速度をこちらで調整
				if (state_ == Atk) {
					player_->SetAnimation(ALPlayer::Punch2, 0.1f, 1, false);
				}
				else if (state_ == Ed) {
					player_->SetAnimation(ALPlayer::Idle, 0.5f, 1, false);
				}
			}
		}
		else	if (parameters_.atkCount_ == 2) {
			//初期化処理が行われていなければ初期化
			if (!parameters_.isInit) {
				parameters_.isInit = true;
				//アニメーションをセット
				player_->SetAnimation(ALPlayer::PrePunch3, 0.5f, 1, false);

			}
			else {
				//アニメーションの更新速度をこちらで調整
				if (state_ == Atk) {
					player_->SetAnimation(ALPlayer::Punch3, 0.1f, 1, false);
				}
				else if (state_ == Ed) {
					player_->SetAnimation(ALPlayer::Idle, 0.5f, 1, false);
				}
			}
		}

		//攻撃入力が含まれていた場合
		if (!parameters_.isNextInput && player_->GetATKInput()) {
			parameters_.isNextInput = true;
		}


		//次段が有効＆次段入力済なら実行
		if (state_==Ed&&(parameters_.atkCount_+1) < maxAtkCount_ && parameters_.isNextInput) {
			//フラグを切る
			parameters_.isNextInput = false;
			parameters_.isInit = false;
			//状態を初期化
			state_ = St;
			//コンボカウント増加
			parameters_.atkCount_++;

			//向きを切り替える
			bool ans;
			player_->SetInputDirection(ans);

			//攻撃コライダー判定をONに
			player_->GetATKCollider()->isActive_ = true;
		}
	}
	else {
		//終了状態にいっている場合



			//終了する
		isEnd_ = true;


	}

}

void PlayerPunch::CheckNextState()
{
	//変更時同じ処理まとめ用フラグ
	bool isChange = false;
	float t = parameters_.count;
	if (parameters_.atkCount_ == 0) {

		switch (state_)
		{
		case PlayerPunch::St:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com1.stStiffnessSec) {
				isChange = true;
				state_ = Atk;
				parameters_.count = 0;
				//音発生
				AudioManager::PlaySoundData(punchSound_);
			}
			else {
				t /= parameters_.com1.stStiffnessSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::Atk:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com1.atkSec) {
				isChange = true;
				state_ = Ed;
				parameters_.count = 0;
			}
			else {
				t /= parameters_.com1.atkSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::Ed:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com1.edStiffnessSec) {
				isChange = true;
				state_ = CheckEnd;
				parameters_.count = 0;
			}
			else {
				t /= parameters_.com1.edStiffnessSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::_countAtkState:
			break;
		default:
			break;
		}
	}
	else if (parameters_.atkCount_ == 1) {

		switch (state_)
		{
		case PlayerPunch::St:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com2.stStiffnessSec) {
				isChange = true;
				state_ = Atk;
				parameters_.count = 0;
				//音発生
				AudioManager::PlaySoundData(kickSound_);
			}
			else {
				t /= parameters_.com2.stStiffnessSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::Atk:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com2.atkSec) {
				isChange = true;
				state_ = Ed;
				parameters_.count = 0;
			}
			else {
				t /= parameters_.com2.atkSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::Ed:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com2.edStiffnessSec) {
				isChange = true;
				state_ = CheckEnd;
				parameters_.count = 0;
			}
			else {
				t /= parameters_.com2.edStiffnessSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::_countAtkState:
			break;
		default:
			break;
		}
	}
	else if (parameters_.atkCount_ == 2) {

		switch (state_)
		{
		case PlayerPunch::St:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com3.stStiffnessSec) {
				isChange = true;
				state_ = Atk;
				//音発生
				AudioManager::PlaySoundData(drilSound_);
			}
			else {
				t /= parameters_.com3.stStiffnessSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::Atk:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com3.atkSec) {
				isChange = true;
				state_ = Ed;
			}
			else {
				t /= parameters_.com3.atkSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::Ed:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com3.edStiffnessSec) {
				isChange = true;
				state_ = CheckEnd;
			}
			else {
				t /= parameters_.com3.edStiffnessSec;
				player_->SetAnimeTime(true, t);
			}
			break;
		case PlayerPunch::_countAtkState:
			break;
		default:
			break;
		}
	}

	if (isChange) {
		parameters_.count = 0;
		parameters_.isInit = false;
	}

}

