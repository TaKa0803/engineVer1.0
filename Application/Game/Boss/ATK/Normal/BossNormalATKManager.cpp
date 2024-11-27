#include "BossNormalATKManager.h"
#include"Game/Boss/Boss.h"
#include"RandomNum/RandomNum.h"


#pragma region 各攻撃
#include"Game/Boss/ATK/Normal/Stump/BossStump.h"
#include"Game/Boss/ATK/Normal/ShotBullet/BossShotBullet.h"
#include"Game/Boss/ATK/Normal/Charge/BossCharge.h"
#include"Game/Boss/ATK/Normal/Punch/BossPunch.h"
#include"Game/Boss/ATK/Normal/SumerSolt/BossSumerSolt.h"

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
	//typeArr_[(int)ATKType::Punch] = std::make_unique<BossPunch>();
	//typeArr_[(int)ATKType::SumerSolt] = std::make_unique<BossSumerSolt>();
	//デバッグ設定
	tree.SetMonitorValue("ATK type", &nowAtkName_);
	tree.SetMonitorValue("攻撃指定", &isDesignate_);
	tree.SetValue("攻撃指定値", &designateType_);
	
	for (auto& arr : typeArr_) {
		tree.SetTreeData(arr->GetTree());
	}

}

void BossNormalATKManager::Initialize()
{
	//ランダムな技を選択して使用
	//ここでtypeを変える処理
	//思考処理をここに記述

	//一旦確立
	int ans = (int)RandomNumber::Get(0, (int)ATKType::CountOfATKData);
	if (ans == (int)ATKType::CountOfATKData) {
		ans--;
	}
	type_ = (ATKType)ans;

	//デバッグ時のタイプ指定など
	Debug();

	//初期化
	typeArr_[(int)type_]->Init();
	
	//攻撃修了確認処理オフ
	isFinished_ = false;
}

void BossNormalATKManager::Update()
{
	//更新処理
	typeArr_[(int)type_]->Update();
}

void BossNormalATKManager::Debug()
{
#ifdef _DEBUG
	//名前を現在の状態に
	nowAtkName_ = atkTypeName_[type_];

	//攻撃を指定していた場合
	if (isDesignate_) {

		//指定された値が範囲外なら範囲内へ
		if (designateType_ < 0) {
			designateType_ = 0;
		}
		else if (designateType_ >=	CountOfATKData) {
			designateType_ = CountOfATKData - 1;;
		}

		//名前指定
		nowAtkName_ = atkTypeName_[designateType_];

		//タイプ指定
		type_ = (ATKType)designateType_;
	}
#endif // _DEBUG


}


