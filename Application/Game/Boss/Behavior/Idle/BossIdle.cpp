#include "BossIdle.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"


BossIdle::BossIdle(Boss* boss)
{
	//ボスのポインタ取得
	boss_ = boss;
	
	//デバッグ用にパラメータを設定
	tree_.name_ = "idle";
	tree_.SetValue("sec", &data_.idleSec);
	tree_.SetValue("animeTransitionSec", &data_.animeTransitionSec);
	tree_.SetMonitorValue("current sec", &data_.currentSec);
}

void BossIdle::Initialize()
{
	//待機時間カウントリセット
	data_.currentSec = 0;

	//アニメーションセット
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], data_.animeTransitionSec, data_.idleSec);
}

void BossIdle::Update()
{
	//待機終わったら処理
	data_.currentSec += (float)DeltaTimer::deltaTime_;

	//カウント達成の場合
	if (data_.currentSec >= data_.idleSec) {
		////距離によって動き
		//float rand = RandomNumber::Get(0.0f, 1.0f);
		////ランダムで攻撃か移動か変更
		//if (rand < 0.5f) {
		//	boss_->SetBehavior(Boss::Behavior::DOWN);
		//}
		//else {
		//	boss_->SetBehavior(Boss::Behavior::ATK);
		//}
		
		//#TODO：AI処理をここに組み込む又は思考処理を外部に出す

		//現在は攻撃のみ
		boss_->SetBehavior(Boss::Behavior::ATK);
	}
	else {
		//割合求める
		float t = data_.currentSec / data_.idleSec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true,t);
	}
}


