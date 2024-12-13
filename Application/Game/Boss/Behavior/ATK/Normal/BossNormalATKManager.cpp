#include "BossNormalATKManager.h"
#include"Game/Boss/Boss.h"
#include"RandomNum/RandomNum.h"


#pragma region 各攻撃
#include"Game/Boss/Behavior/ATK/Normal/Stump/BossStump.h"
#include"Game/Boss/Behavior/ATK/Normal/ShotBullet/BossShotBullet.h"
#include"Game/Boss/Behavior/ATK/Normal/Charge/BossCharge.h"
#include"Game/Boss/Behavior/ATK/Normal/Punch/BossPunch.h"
#include"Game/Boss/Behavior/ATK/Normal/SumerSolt/BossSumerSolt.h"
#include"Game/Boss/Behavior/ATK/Normal/BackStep/BossBackStep.h"
#pragma endregion


BossNormalATKManager::BossNormalATKManager(Boss* boss)
{
	//ボスのポインタ
	boss_ = boss;

	//サイズ分に合わせる
	typeArr_.resize((size_t)ATKType::CountOfATKData);

	//各攻撃取得
	typeArr_[(int)ATKType::Stump] = std::make_unique<BossStump>();
	typeArr_[(int)ATKType::ShotBullet] = std::make_unique<BossShotBullet>();
	typeArr_[(int)ATKType::Charge] = std::make_unique<BossCharge>();
	//typeArr_[(int)ATKType::Punch] = std::make_unique<BossPunch>();
	//typeArr_[(int)ATKType::SumerSolt] = std::make_unique<BossSumerSolt>();
	typeArr_[(int)ATKType::BackStep] = std::make_unique<BossBackStep>();


	//デバッグ用にパラメータをツリーに入れる
	//デバッグ用に文字列をまとめる
	std::vector<std::string> atkname;
	atkname.resize((int)ATKType::CountOfATKData);
	for (int i = 0; i < ATKType::CountOfATKData; i++) {
		atkname[i] = atkTypeName_[i];
	}
	//デバッグ設定
	tree.SetMonitorValue("ATK type", &nowAtkName_);
	tree.SetMonitorValue("攻撃指定", &isDesignate_);
	tree.SetMonitorCombo("攻撃指定値", &designateType_,atkname);
	for (auto& arr : typeArr_) {
		tree.SetTreeData(arr->GetTree());
	}
}

void BossNormalATKManager::Initialize(std::optional<int>contract)
{
	//ランダムな技を選択して使用
	//ここでtypeを変える処理
	//思考処理をここに記述

	//一旦確率
	int ans = (int)RandomNumber::Get(0, (int)ATKType::CountOfATKData);
	//範囲外なら一つ下に
	if (ans == (int)ATKType::CountOfATKData) {
		ans--;
	}
	//結果を与える
	type_ = (ATKType)ans;

	//デバッグ時のタイプ指定など
	Debug();

	//攻撃を指定されていた場合その値にする
	if (contract) {
		type_ = (ATKType)contract.value();
	}

	////初期化
	typeArr_[(int)type_]->Initialize();
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
			designateType_ = CountOfATKData - 1;
		}

		//デバッグ用名前を設定
		nowAtkName_ = atkTypeName_[designateType_];

		//タイプ指定
		type_ = (ATKType)designateType_;
	}
#endif // _DEBUG


}


