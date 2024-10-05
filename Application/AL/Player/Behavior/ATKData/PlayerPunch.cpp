#include "PlayerPunch.h"
#include"AL/Player/ALPlayer.h"
#include"DeltaTimer/DeltaTimer.h"

PlayerPunch::PlayerPunch(ALPlayer* player)
{
	player_ = player;
}

void PlayerPunch::Initialize()
{
	//攻撃の状態取得
	AtkState state_ = St;
	isEnd_ = false;

	//各シーンカウント
	parameters_.count = 0;
	//初期化処理
	parameters_.isInit = false;
	//連撃回数
	parameters_.atkCount_ = 0;


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

			}
			else {
				//アニメーションの更新速度をこちらで調整

			}

		}

	}
	else {
		//いっている場合

		//次段が有効＆次段入力済なら実行
		if (parameters_.atkCount_++<maxAtkCount_&&parameters_.isNextInput) {
			//フラグを切る
			parameters_.isNextInput = false;
			parameters_.isInit = false;
			//状態を初期化
			state_ = St;
		}
		else {
			//終了する
			

		}
	}

}

void PlayerPunch::CheckNextState()
{
	//変更時同じ処理まとめ用フラグ
	bool isChange = false;

	if (parameters_.atkCount_ == 0) {
		switch (state_)
		{
		case PlayerPunch::St:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com1.stStiffnessSec) {
				isChange = true;
				state_ = Atk;
			}
			break;
		case PlayerPunch::Atk:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com1.atkSec) {
				isChange = true;
				state_ = Ed;
			}
			break;
		case PlayerPunch::Ed:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com1.edStiffnessSec) {
				isChange = true;
				state_ = CheckEnd;
			}
			break;
		case PlayerPunch::_countAtkState:
			break;
		default:
			break;
		}
	}else if (parameters_.atkCount_ == 1) {

		switch (state_)
		{
		case PlayerPunch::St:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com2.stStiffnessSec) {
				isChange = true;
				state_ = Atk;
			}
			break;
		case PlayerPunch::Atk:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com2.atkSec) {
				isChange = true;
				state_ = Ed;
			}
			break;
		case PlayerPunch::Ed:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com2.edStiffnessSec) {
				isChange = true;
				state_ = CheckEnd;
			}
			break;
		case PlayerPunch::_countAtkState:
			break;
		default:
			break;
		}
	}else if (parameters_.atkCount_ == 2) {

		switch (state_)
		{
		case PlayerPunch::St:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com3.stStiffnessSec) {
				isChange = true;
				state_ = Atk;
			}
			break;
		case PlayerPunch::Atk:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com3.atkSec) {
				isChange = true;
				state_ = Ed;
			}
			break;
		case PlayerPunch::Ed:
			//交直時間経過で次の状態へ
			if (parameters_.count >= parameters_.com3.edStiffnessSec) {
				isChange = true;
				state_ = CheckEnd;
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

