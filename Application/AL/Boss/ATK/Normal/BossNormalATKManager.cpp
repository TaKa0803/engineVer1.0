#include "BossNormalATKManager.h"
#include"AL/Boss/Boss.h"
#include"RandomNum/RandomNum.h"

#pragma region 各攻撃
#include"AL/Boss/ATK/Normal/Stump/BossStump.h"
#include"AL/Boss/ATK/Normal/Charge/BossCharge.h"
#pragma endregion


BossNormalATKManager::BossNormalATKManager(Boss* boss)
{
	boss_ = boss;

	typeArr_.resize((size_t)ATKType::CountOfATKData);

	typeArr_[(int)ATKType::Stump] = std::make_unique<BossStump>();
	typeArr_[(int)ATKType::Charge] = std::make_unique<BossCharge>();

}

void BossNormalATKManager::Initialize()
{
	//ランダムな技を選択して使用
	//ここでtypeを変える処理
	//思考処理をここに記述
	if (RandomNumber::Get(0, 1) > 0.5f) {
		type_ = ATKType::Stump;

	}
	else {
		type_ = ATKType::Charge;

	}

	//type_ = ATKType::Charge;

	//初期化
	typeArr_[(int)type_]->Init();
	
	isFinished_ = false;
}

void BossNormalATKManager::Update()
{
	//更新処理
	typeArr_[(int)type_]->Update();
}


