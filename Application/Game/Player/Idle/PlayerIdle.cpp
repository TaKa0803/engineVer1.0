#include "PlayerIdle.h"
#include"Game/Player/Player.h"

PlayerIdle::PlayerIdle(Player* player)
{
	//ポインタ取得
	player_ = player;
}

void PlayerIdle::Initialize()
{
	//アニメーションセット
	player_->SetAnimation(player_->animeName_[(int)Player::Behavior::IDLE], 0.2f, 1.0f);
}

void PlayerIdle::Update()
{
	//移動処理
	player_->Move();

	//入力による状態変化処理
	player_->ChangeBehavior();
}
