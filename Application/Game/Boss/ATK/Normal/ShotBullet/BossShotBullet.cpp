#include "BossShotBullet.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossShotBullet::BossShotBullet()
{

	bulletM_ = BossBulletManager::GetInstance();

	//各状態時間設定
	parameters_.aimingSec = 0.5f;
	parameters_.warningSec = 2.0f;

	//デバッグ設定
	treeData_.SetName("弾発射");

	treeData_.SetValue("弾の数", &shotNum_);
	treeData_.SetValue("ターゲット時間/s", &parameters_.aimingSec);
	treeData_.SetValue("発射時間/s", &parameters_.warningSec);
	treeData_.SetValue("復帰時間/s", &parameters_.backSec);
}



void BossShotBullet::InitAIMing()
{
	boss_->SetAnimeTime(true);
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Preshot], 1, 1);
}

void BossShotBullet::InitWarning()
{
	//発射カウント初期化
	shotCount_ = 0;

	//boss_->SetAnimation((int)Boss::Animation::Shot, 1, 1,false);
}

void BossShotBullet::InitBack()
{

	boss_->SetAnimeTime(false);
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], 1, 1,false);
}

void BossShotBullet::UpdateAIMing()
{
	//向きを設定
	boss_->SetDirection2Player();

	//カウント完了で次へ
	if (currentCount_ >= parameters_.aimingSec) {
		behaviReq_ = Warning;
	}
	else {
		float t = currentCount_ / parameters_.aimingSec;
		boss_->SetAnimeTime(true, t);
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

			Joint joint;
			if ((int)shotNum_ % 2 == 0) {
				joint = boss_->model_->GetJoint(rHandBoneName_);
			}
			else {
				joint = boss_->model_->GetJoint(lHandBoneName_);
			}

			//ジョイントのワールド座標取得
			Matrix4x4 world = joint.skeletonSpaceMatrix;
			EulerWorldTransform jointWT;
			jointWT.matWorld_ = world * boss_->world_.matWorld_;


			//ジョイントワールド座標を出現地点に
			newData.world.translate_ = jointWT.GetWorldTranslate();

			//プレイヤー方向に発射
			newData.velo = boss_->GetPlayerWorldTranslate() - newData.world.translate_;
			//データ追加
			bulletM_->SetData(newData);

			//発生カウント増加
			shotCount_++;

			if (0 == (int)shotCount_ % 2) {
				boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::RevShot], 0.1f, 1, false);
			}
			else {
				boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Shot], 0.1f, 1, false);
			}
		}
		else {

			float t = (currentCount_- (threshold * shotCount_)) / parameters_.warningSec;
			boss_->SetAnimeTime(true, t);
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
