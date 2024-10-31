#include "PlayerATKManager.h"
#include"Game/Player/ALPlayer.h"

PlayerATKManager::PlayerATKManager(ALPlayer* player) {
	player_ = player;

	punch_ = std::make_unique<PlayerPunch>(player);
}

void PlayerATKManager::Initialize()
{
	isEnd_ = false;
	//状態の初期化
	(this->*TypeInit[(int)type_])();
}

void PlayerATKManager::Update()
{
	//状態の更新
	(this->*TypeUpdate[(int)type_])();

	if (type_ == Punch&&punch_->isEnd_) {
		isEnd_ = true;
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
