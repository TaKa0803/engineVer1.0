#include "BossShotBullet.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossShotBullet::BossShotBullet()
{
	//弾マネージャのインスタンス取得
	bulletM_ = BossBulletManager::GetInstance();

	//デバッグ用にパラメータを設定
	treeData_.name_ = "弾発射";
	treeData_.SetValue("弾の数", &shotNum_);
	treeData_.SetValue("ターゲット時間/s", &parameters_.aimingSec);
	treeData_.SetValue("発射時間/s", &parameters_.warningSec);
	treeData_.SetValue("復帰時間/s", &parameters_.backSec);
}



void BossShotBullet::InitAIMing()
{
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Preshot], parameters_.aimingSec, 1,false);
}

void BossShotBullet::InitWarning()
{
	//発射カウント初期化
	shotCount_ = 0;

	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Shot], parameters_.warningSec, 1,false);
}

void BossShotBullet::InitBack()
{
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], parameters_.backSec, 1,false);
}

void BossShotBullet::UpdateAIMing()
{
	//向きを設定
	boss_->SetDirection2Player();

	//カウント完了で次へ
	if (currentCount_ >= parameters_.aimingSec) {
		//次の状態へ
		behaviReq_ = Warning;
	}
	else {
		//割合取得
		float t = currentCount_ / parameters_.aimingSec;
		//アニメーション再生をで管理
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

			//ジョイントの座標設定
			Matrix4x4 joint;
			//左右どちらの手のノード座標取得
			if ((int)shotCount_ % 2 == 0) {
				//左手のノードの行列取得
				joint = boss_->model_->GetJoint(lHandBoneName_);
			}
			else {
				//右手のノードの行列取得
				joint = boss_->model_->GetJoint(rHandBoneName_);
			}

			//ジョイントのワールド座標取得
			EulerWorldTransform jointWT;
			jointWT.UpdateMatrix();
			
			jointWT.matWorld_ *= joint;

			//ジョイントワールド座標を出現地点に
			newData.world.translate_ = jointWT.GetWorldTranslate();

			//プレイヤー方向のベクトルを取得
			newData.velo = boss_->GetPlayerWorldTranslate() - newData.world.translate_;

			//データ追加
			bulletM_->SetData(newData);

			//発生カウント増加
			shotCount_++;
		}
		else {
			//０-１の進行度T作成
			float t;

			//
			if (0 == (int)shotCount_ % 2) {
				//０-＞１に値変化
				t = currentCount_ /( threshold * (shotCount_));
			}
			else {
				//１-＞０に値変化
				t =1.0f- currentCount_ / (threshold * (shotCount_));
			}

			//アニメーション再生をTで管理
			boss_->SetAnimeTime(true, t);
		}
	}
}

void BossShotBullet::UpdateATK()
{
	//弾がすべて消えたら元に戻る処理
	if (bulletM_->GetBulletCount() == 0) {
		//状態を変更
		behaviReq_ = Back;
	}
}

void BossShotBullet::UpdateBack()
{
	//カウント完了で終わり
	if (currentCount_ >= parameters_.backSec) {
		//終了フラグをON
		boss_->isFinishedATK_ = true;
	}
}
