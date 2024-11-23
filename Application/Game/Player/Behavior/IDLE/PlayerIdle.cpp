#include "PlayerIdle.h"
#include"Game/Player/Player.h"

PlayerIdle::PlayerIdle(Player* player)
{
}

void PlayerIdle::Initialize()
{
}

void PlayerIdle::Update()
{
	////移動処理
	//player_->Move();

	////攻撃状態,回転処理に移るかの処理
	//bool isATK, isRoll = { false };

	////各入力取得
	//isATK = GetATKInput();
	//isRoll = GetRollInput();

	//if (isATK && data_.stamina.currentStamina >= data_.stamina.atkCost) {
	//	behaviorReq_ = State::ATK;
	//}

	//if (isRoll && data_.stamina.currentStamina >= data_.stamina.rollCost) {
	//	behaviorReq_ = State::Rolling;
	//}
}
