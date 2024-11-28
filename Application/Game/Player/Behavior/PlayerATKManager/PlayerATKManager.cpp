#include "PlayerATKManager.h"
#include"Game/Player/Player.h"

PlayerATKManager::PlayerATKManager(Player* player) {
	player_ = player;

	punch_ = std::make_unique<PlayerPunch>(player);

	tree_.name_ = "プレイヤ攻撃マネージャ";
	tree_.SetMonitorValue("攻撃状態", &nowType_);
	tree_.SetMonitorValue("終了取得", &isEnd_);
	tree_.SetTreeData(punch_->GetTree());
}

void PlayerATKManager::Initialize()
{
	isEnd_ = false;
	//状態の初期化
	(this->*TypeInit[(int)type_])();

	nowType_ = typeName_[(int)type_];
}

void PlayerATKManager::Update()
{
	//状態の更新
	(this->*TypeUpdate[(int)type_])();


	//攻撃処理が終わったフラグを取得
	if (type_ == Punch && punch_->isEnd_) {
		player_->SetBehaviorReq(Player::Behavior::IDLE);
	}
}

void (PlayerATKManager::* PlayerATKManager::TypeInit[])() {
	&PlayerATKManager::InitPunch
};
void (PlayerATKManager::* PlayerATKManager::TypeUpdate[])() {
	&PlayerATKManager::UpdatePunch
};

#pragma region 各状態
void PlayerATKManager::InitPunch() { punch_->Initialize(); }
void PlayerATKManager::UpdatePunch() { punch_->Update(); }
#pragma endregion
