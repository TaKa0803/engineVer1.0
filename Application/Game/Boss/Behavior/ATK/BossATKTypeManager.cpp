#include "BossATKTypeManager.h"
#include"Game/Boss/Boss.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"


BossATKTypeManager::BossATKTypeManager(Boss* boss)
{
	//ボスのポインタ取得
	boss_ = boss;
	//通常攻撃の生成
	normal_ = std::make_unique<BossNormalATKManager>(boss);

	//デバッグ用にパラメータ設定
	std::unique_ptr<GlobalVariableGroup> gvg = std::make_unique<GlobalVariableGroup>("BossATKManager");
	gvg->SetMonitorValue("behavior", &nowTypeName_);
	gvg->SetTreeData(normal_->GetTree());

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
	(this->*TypeInit[(int)modeType])(plannedATK_);

	//もし値がある場合消す
	if (plannedATK_) {
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
	(this->*TypeUpdate[(int)modeType])();

	//攻撃が終わった場合の処理
	//追い討ちなどかけない場合は素直にIDLE状態へ
	if (boss_->isFinishedATK_) {
		boss_->isFinishedATK_ = false;
		//とりあえずの命令終了
		boss_->SetBehavior(Boss::Behavior::IDLE);
	}
}

//TODO関数テーブルからポリモーフィズムに変更
void (BossATKTypeManager::* BossATKTypeManager::TypeInit[])(std::optional<int>contract) {
	&BossATKTypeManager::InitNormal,
};
void (BossATKTypeManager::* BossATKTypeManager::TypeUpdate[])() {
	&BossATKTypeManager::UpdateNormal
};

#pragma region 各状態の初期化と更新
//TODO関数テーブルからポリモーフィズムに変更
void BossATKTypeManager::InitNormal(std::optional<int>contract) { normal_->Initialize(contract); }
void BossATKTypeManager::UpdateNormal() { normal_->Update(); }
#pragma endregion

