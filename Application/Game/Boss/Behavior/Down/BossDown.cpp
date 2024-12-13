#include"BossDown.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossDown::BossDown(Boss*boss)
{
	//ボスポインタ取得
	boss_ = boss;

	//デバッグ用にパラメータをツリーに追加
	tree_.name_ = "ダウン";
	tree_.SetValue("ダウンアニメーション", &animeSec_);
	tree_.SetValue("次に行くまでの時間", &nextSec_);
}

void BossDown::Initialize() {
	//アニメーションセット
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Down], animeSec_, animeSec_, false);
	//次までのカウント初期化
	currentNext_ = 0;
}

void BossDown::Update() {
	//カウント追加
	currentNext_ += (float)DeltaTimer::deltaTime_;

	//カウント以上で返却
	if (currentNext_ >= nextSec_) {
		//死亡フラグON
		boss_->isDead_ = true;
	}
}