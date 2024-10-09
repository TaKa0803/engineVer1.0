#include "BossShotBullet.h"
#include"AL/Boss/Boss.h"

BossShotBullet::BossShotBullet()
{
	//各状態時間設定
	parameters_.aimingSec = 0.5f;
	parameters_.warningSec = 1.0f;
}



void BossShotBullet::InitAIMing()
{

	currentCount_ = 0;

}

void BossShotBullet::InitWarning()
{
	currentCount_ = 0;
}

void BossShotBullet::InitATK()
{
	//弾がすべて消えたら元に戻る処理へ
}

void BossShotBullet::InitStiffness()
{
}

void BossShotBullet::InitBack()
{
	currentCount_ = 0;
}


void BossShotBullet::UpdateAIMing()
{
	//向きを設定
	boss_->SetDirection2Player();
}

void BossShotBullet::UpdateWarning()
{
	//向きを設定
	boss_->SetDirection2Player();

	//プレイヤー方向に弾を発射

}

void BossShotBullet::UpdateATK()
{
}

void BossShotBullet::UpdateStiffness()
{
}

void BossShotBullet::UpdateBack()
{
}
