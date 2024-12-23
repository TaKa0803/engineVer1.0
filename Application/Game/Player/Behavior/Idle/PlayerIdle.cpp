#include "PlayerIdle.h"
#include"Game/Player/Player.h"

PlayerIdle::PlayerIdle(Player* player)
{
	//ポインタ取得
	player_ = player;

	//デバッグ用ツリー設定
	tree_.name_ = "通常状態";
	tree_.SetValue("アニメーション補間",&complementSec_ );
	tree_.SetValue("再生速度倍率", &animeSecMulti_);
}

void PlayerIdle::Initialize()
{
	//アニメーションセット
	player_->animeManager_->SetAnimation(PAnimeM::Idle, complementSec_, animeSecMulti_);
}

void PlayerIdle::Update()
{
	//移動処理
	player_->Move();
	//入力による状態変化処理
	player_->ChangeBehavior();
}
