#include "BossShotBullet.h"
#include"AL/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossShotBullet::BossShotBullet()
{

	bulletM_ = BossBulletManager::GetInstance();

	//各状態時間設定
	parameters_.aimingSec = 0.5f;
	parameters_.warningSec = 2.0f;
}



void BossShotBullet::InitWarning()
{
	//発射カウント初期化
	shotCount_ = 0;
}

void BossShotBullet::UpdateAIMing()
{
	//向きを設定
	boss_->SetDirection2Player();

	//カウント完了で次へ
	if (currentCount_ >= parameters_.aimingSec) {
		behaviReq_ = Warning;
	}
}

void BossShotBullet::UpdateWarning()
{
	//向きを設定
	boss_->SetDirection2Player();


	//カウント完了で次へ
	if (currentCount_ >= parameters_.warningSec) {
		behaviReq_ = ATK;
	}
	else {
		//カウント未達成なので処理
	
		// 間隔分の値
		float threshold = parameters_.warningSec / shotNum_;

		//現在の値が超えれば発生
		if (currentCount_ >= threshold * (shotCount_)) {
			//プレイヤー方向に弾を発射
			// 
			//データ作成
			BossBulletData newData;
			//データセット
			newData.world.translate_ = boss_->GetWorld().translate_;
			//プレイヤー方向に発射
			newData.velo = boss_->GetPlayerWorldTranslate() - newData.world.translate_;
			//データ追加
			bulletM_->SetData(newData);

			//発生カウント増加
			shotCount_++;
		}
	}
}

void BossShotBullet::UpdateATK()
{
	//弾がすべて消えたら元に戻る処理
	if (bulletM_->GetBulletCount() == 0) {
		behaviReq_ = Back;
	}
}

void BossShotBullet::UpdateStiffness()
{
}

void BossShotBullet::UpdateBack()
{

	//カウント完了で終わり
	if (currentCount_ >= parameters_.backSec) {
		boss_->isFinishedATK_ = true;
	}
}
