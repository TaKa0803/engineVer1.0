#include "PlayerATKManager.h"
#include"AL/Player/ALPlayer.h"

PlayerATKManager::PlayerATKManager(ALPlayer* player) {
	player_ = player;
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
}

void (PlayerATKManager::* PlayerATKManager::TypeInit[])() {
	&PlayerATKManager::InitPunch
};
void (PlayerATKManager::* PlayerATKManager::TypeUpdate[])() {
	&PlayerATKManager::UpdatePunch
};

#pragma region 各状態
void PlayerATKManager::InitPunch(){}
void PlayerATKManager::UpdatePunch(){}
#pragma endregion
