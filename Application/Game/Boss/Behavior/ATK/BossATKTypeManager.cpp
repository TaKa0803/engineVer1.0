#include "BossATKTypeManager.h"
#include"Game/Boss/Boss.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"

#include"Game/Boss/Behavior/ATK/Normal/BossNormalATKManager.h"

BossATKTypeManager::BossATKTypeManager(Boss* boss)
{
	//ボスのポインタ取得
	boss_ = boss;
	//サイズ分拡張
	behaviors_.resize(CountModeType);
	//通常攻撃の生成
	behaviors_[Normal]= std::make_unique<BossNormalATKManager>(boss);

	//デバッグ用にパラメータ設定
	std::unique_ptr<GlobalVariableGroup> gvg = std::make_unique<GlobalVariableGroup>("BossATKManager");
	gvg->SetMonitorValue("behavior", &nowTypeName_);
	gvg->SetTreeData(behaviors_[Normal]->GetTree());
}

void BossATKTypeManager::SceneInit()
{
	//攻撃タイプ設定
	modeType = Normal;
}

void BossATKTypeManager::Initialize()
{
	SceneInit();
	//攻撃終了フラグをOFF
	boss_->isFinishedATK_ = false;

	//実際の初期化処理
	behaviors_[Normal]->Initialize();

	//もし値がある場合消す
	if (plannedATK_) {
		behaviors_[Normal]->SetATKNum(plannedATK_.value());
		plannedATK_ = std::nullopt;
	}
}

void BossATKTypeManager::Update()
{

#ifdef _DEBUG
	//現在の攻撃タイプの名前取得
	nowTypeName_ = typeName_[modeType];
#endif // _DEBUG

	//状態の更新
	behaviors_[Normal]->Update();

	//攻撃が終わった場合の処理
	//追い討ちなどかけない場合は素直にIDLE状態へ
	if (boss_->isFinishedATK_) {
		boss_->isFinishedATK_ = false;
		//とりあえずの命令終了
		boss_->SetBehavior(Boss::Behavior::IDLE);
	}
}
