#include "PlayerAnimationManager.h"
#include"Game/Player/Player.h"

PlayerAnimationManager::PlayerAnimationManager(Player* player)
{
	//プレイヤーポインタ設定
	player_ = player;
}

void PlayerAnimationManager::SetAnimation(AnimationData data, float sec, float loopSecMulti, bool loop)
{
	//アニメーションを変更
	player_->model_->ChangeAnimation(animeName_[data], sec);
	//アニメーション速度倍率変更
	player_->model_->animationRoopSecond_ = loopSecMulti;
	//ループフラグを変更
	player_->model_->SetAnimationRoop(loop);
}

void PlayerAnimationManager::SetAnimeTime(bool active, float t)
{
	//アニメーション再生進行度を変更
	player_->model_->SetAnimationTime(active, t);
}
