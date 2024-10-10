#include "BossNormalATKManager.h"
#include"AL/Boss/Boss.h"
#include"RandomNum/RandomNum.h"


#pragma region 各攻撃
#include"AL/Boss/ATK/Normal/Stump/BossStump.h"
#include"AL/Boss/ATK/Normal/ShotBullet/BossShotBullet.h"
#include"AL/Boss/ATK/Normal/Charge/BossCharge.h"
#pragma endregion


BossNormalATKManager::BossNormalATKManager(Boss* boss)
{
	boss_ = boss;

	//サイズ分に合わせる
	typeArr_.resize((size_t)ATKType::CountOfATKData);

	//各攻撃取得
	typeArr_[(int)ATKType::Stump] = std::make_unique<BossStump>();
	typeArr_[(int)ATKType::ShotBullet] = std::make_unique<BossShotBullet>();
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

	type_ = ATKType::ShotBullet;

	//初期化
	typeArr_[(int)type_]->Init();
	
	isFinished_ = false;
}

void BossNormalATKManager::Update()
{
	//更新処理
	typeArr_[(int)type_]->Update();
}


