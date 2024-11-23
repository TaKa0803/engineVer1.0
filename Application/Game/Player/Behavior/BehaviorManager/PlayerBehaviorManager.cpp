#include "PlayerBehaviorManager.h"
#include"Game/Player/Player.h"

#include"Game/Player/Behavior/PlayerRoll/PlayerRoll.h"
#include"Game/Player/Behavior/PlayerATKManager/PlayerATKManager.h"
#include"Game/Player/Behavior/Down/PlayerDown.h"




PlayerBehaviorManager::PlayerBehaviorManager(Player* player)
{
	input_ = Input::GetInstance();

	player_ = player;

	behaviors_[(int)Behavior::ROLLING]	 = std::make_unique<PlayerRoll>(this);
	behaviors_[(int)Behavior::ATK]		 = std::make_unique<PlayerATKManager>(this);
	behaviors_[(int)Behavior::HITACTION] = std::make_unique<PlayerDown>(this);


}

void PlayerBehaviorManager::Initilaize()
{
	behaviorReq_ = Behavior::IDLE;
}

void PlayerBehaviorManager::Update()
{
	//状態の初期化処理
	if (behaviorReq_) {
		//状態を変更してリクエストを消す
		behavior_ = behaviorReq_.value();
		behaviorReq_ = std::nullopt;

		//攻撃、ローリング後はアニメーションのフラグが変わるのでここで変更しておく
		//SetAnimeTime(false);

		//実際の初期化処理
		behaviors_[(int)behavior_]->Initialize();
	}

	//状態の更新
	behaviors_[(int)behavior_]->Update();
}

bool PlayerBehaviorManager::GetATKInput()
{
	int isATK = 0;

	isATK += (int)input_->TriggerKey(DIK_Z);

	//コントローラーがあるときの処理
	if (input_->IsControllerActive()) {
		isATK += (int)input_->IsTriggerButton(kButtonB);
	}

	if (isATK > 1) {
		isATK = 1;
	}

	return (bool)isATK;
}

bool PlayerBehaviorManager::GetRollInput()
{
	int ans = input_->TriggerKey(DIK_SPACE);

	if (input_->IsControllerActive()) {
		ans += input_->IsPushButton(kButtonA);
	}

	return ans;
}

bool PlayerBehaviorManager::GetDashInput()
{
	int ans = input_->PushKey(DIK_LSHIFT);
	if (input_->IsControllerActive()) {
		ans += input_->IsPushButton(kButtonX);
	}
	return ans;
}
