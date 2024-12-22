#include "PlayerPunch.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AudioManager/AudioManager.h"

PlayerPunch::PlayerPunch(Player* player)
{
	//プレイヤーのポインタ取得
	player_ = player;

	//各動きの音の配列番号取得
	punchSound_ = AudioManager::LoadSoundNum("com1");
	kickSound_ = AudioManager::LoadSoundNum("com2");
	drilSound_ = AudioManager::LoadSoundNum("com3");

	//ツリーの名前設定
	tree_.name_ = "パンチ";

	GlobalVariableTree com1T = GlobalVariableTree("コンボ1");
	com1T.SetValue("移動倍率", &parameters_.com1.multiSpd);
	com1T.SetValue("開始交直", &parameters_.com1.stStiffnessSec);
	com1T.SetValue("攻撃時間", &parameters_.com1.atkSec);
	com1T.SetValue("終了交直", &parameters_.com1.edStiffnessSec);

	GlobalVariableTree com2T = GlobalVariableTree("コンボ2");
	com2T.SetValue("移動倍率", &parameters_.com2.multiSpd);
	com2T.SetValue("開始交直", &parameters_.com2.stStiffnessSec);
	com2T.SetValue("攻撃時間", &parameters_.com2.atkSec);
	com2T.SetValue("終了交直", &parameters_.com2.edStiffnessSec);

	GlobalVariableTree com3T = GlobalVariableTree("コンボ3");
	com3T.SetValue("移動倍率", &parameters_.com3.multiSpd);
	com3T.SetValue("開始交直", &parameters_.com3.stStiffnessSec);
	com3T.SetValue("攻撃時間", &parameters_.com3.atkSec);
	com3T.SetValue("終了交直", &parameters_.com3.edStiffnessSec);

	tree_.SetTreeData(com1T);
	tree_.SetTreeData(com2T);
	tree_.SetTreeData(com3T);

}

void PlayerPunch::Initialize()
{
	//攻撃の状態取得
	state_ = St;

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

	//パラメータに値を追加
	Com1& com = parameters_.com1;
	parameters_.multiSpd = com.multiSpd;
	parameters_.stStiffness = com.stStiffnessSec;
	parameters_.atk = com.atkSec;
	parameters_.edStiffness = com.edStiffnessSec;

	//攻撃コライダー判定をONに
	player_->GetATKCollider()->isActive_ = true;
	//kougekiji
	player_->InitATK();
}

void PlayerPunch::Update()
{
	//各状態の値加算値加算
	parameters_.count += (float)DeltaTimer::deltaTime_;

	//攻撃の始動、終了の処理
	CheckNextState();

	//終了状態までいっていない場合
	if (state_ != CheckEnd) {
		//各状態でのアニメーションの変更
		ChangeAnimation();

		//攻撃入力が含まれていた場合
		if (!parameters_.isNextInput && player_->GetATKInput()) {
			parameters_.isNextInput = true;
		}


		//次段が有効＆次段入力済なら実行
		if (state_ == Ed && (parameters_.atkCount_ + 1) < maxAtkCount_ && parameters_.isNextInput&&player_->GetStaminaOfATK()) {
			//フラグを切る
			parameters_.isNextInput = false;
			parameters_.isInit = false;
			//状態を初期化
			state_ = St;
			//コンボカウント増加
			parameters_.atkCount_++;

			//各パラの変更
			if (parameters_.atkCount_ == 1) {
				Com2& com = parameters_.com2;
				parameters_.multiSpd = com.multiSpd;
				parameters_.stStiffness = com.stStiffnessSec;
				parameters_.atk = com.atkSec;
				parameters_.edStiffness = com.edStiffnessSec;
			}
			else if (parameters_.atkCount_ == 2) {
				Com3& com = parameters_.com3;
				parameters_.multiSpd = com.multiSpd;
				parameters_.stStiffness = com.stStiffnessSec;
				parameters_.atk = com.atkSec;
				parameters_.edStiffness = com.edStiffnessSec;
			}

			//向きを切り替える
			bool ans;
			player_->SetInputDirection(ans);

			player_->InitATK();
		}
	}
	else {
		//終了状態にいっている場合
		player_->data_.isEndATK_ = true;
	}

}

void PlayerPunch::CheckNextState()
{
	//変更時同じ処理まとめ用フラグ
	bool isChange = false;
	float t = parameters_.count;

	switch (state_)
	{
	case PlayerPunch::St:
		//交直時間経過で次の状態へ
		if (t >= parameters_.stStiffness) {
			isChange = true;
			state_ = Atk;
			//攻撃コライダー判定をONに
			player_->GetATKCollider()->isActive_ = true;
			//音発生
			AudioManager::PlaySoundData(punchSound_);
			player_->SetAnimeTime(true, 1);

		}
		else {
			t /= parameters_.stStiffness;
			player_->SetAnimeTime(true, t);
			player_->Move(false, parameters_.multiSpd);
		}
		break;

	case PlayerPunch::Atk:
		//交直時間経過で次の状態へ
		if (t >= parameters_.atk) {
			isChange = true;
			state_ = Ed;
			//攻撃コライダー判定をONに
			player_->GetATKCollider()->isActive_ = false;
			player_->SetAnimeTime(true, 1);
		}
		else {
			t /= parameters_.atk;
			player_->SetAnimeTime(true, t);
		}
		break;

	case PlayerPunch::Ed:
		//交直時間経過で次の状態へ
		if (t >= parameters_.edStiffness) {
			isChange = true;
			state_ = CheckEnd;
			player_->SetAnimeTime(true, 1);
		}
		else {
			t /= parameters_.edStiffness;
			//構えた状態でをフリーズ
			player_->SetAnimeTime(true, 1);
		}
		break;
	case PlayerPunch::CountAtkState:
		break;
	default:
		break;
	}

	if (isChange) {
		parameters_.count = 0;
		parameters_.isInit = false;
	}

}

void PlayerPunch::ChangeAnimation()
{
	//攻撃が一段目の場合
	if (parameters_.atkCount_ == 0) {
		//初期化処理が行われていなければ初期化
		if (!parameters_.isInit) {
			parameters_.isInit = true;
			//アニメーションをセット
			player_->SetAnimation(player_->animeName_[Player::PrePunch1], 0, 1, false);

			//アニメーションの更新速度をこちらで調整
			if (state_ == Atk) {
				player_->SetAnimation(player_->animeName_[Player::Punch1], 0, 1, false);
			}
			else if (state_ == Ed) {
				player_->SetAnimation(player_->animeName_[Player::Punch1], 0, 1, false);
			}
		}

	}
	else if (parameters_.atkCount_ == 1) {
		//初期化処理が行われていなければ初期化
		if (!parameters_.isInit) {
			parameters_.isInit = true;
			//アニメーションをセット
			player_->SetAnimation(player_->animeName_[Player::PrePunch2], 0, 1, false);
			//アニメーションの更新速度をこちらで調整
			if (state_ == Atk) {
				player_->SetAnimation(player_->animeName_[Player::Punch2], 0, 1, false);
			}
			else if (state_ == Ed) {
				player_->SetAnimation(player_->animeName_[Player::Punch2], 0, 1, false);
			}
		}
	}
	else if (parameters_.atkCount_ == 2) {
		//初期化処理が行われていなければ初期化
		if (!parameters_.isInit) {
			parameters_.isInit = true;
			//アニメーションをセット
			player_->SetAnimation(player_->animeName_[Player::PrePunch3], parameters_.com2.stStiffnessSec, 1, false);

			//アニメーションの更新速度をこちらで調整
			if (state_ == Atk) {
				player_->SetAnimation(player_->animeName_[Player::Punch3], 0, 1, false);
			}
			else if (state_ == Ed) {
				player_->SetAnimation(player_->animeName_[Player::Idle], parameters_.com3.edStiffnessSec, 1, false);
			}
		}
	}
}

