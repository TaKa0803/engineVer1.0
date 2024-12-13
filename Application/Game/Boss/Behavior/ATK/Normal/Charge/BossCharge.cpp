#include "BossCharge.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game//Boss/Boss.h"
#include<numbers>

BossCharge::BossCharge()
{
	//デバッグ用にパラメータをツリーに追加
	treeData_.name_="タックル";
	treeData_.SetValue("狙う時間/s", &parameters_.aimingSec);
	treeData_.SetValue("警告時間/s", &parameters_.warningSec);
	treeData_.SetValue("ダッシュ速度", &dash_.spd);
	treeData_.SetValue("ダッシュアニメーション速度", &dash_.animeSpd);

	treeData_.SetValue("硬直時間/s", &parameters_.stiffnessSec);
	treeData_.SetValue("復帰時間/s", &parameters_.backSec);
}

void BossCharge::InitAIMing() {
	//アニメーションをセット
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PreCharge], parameters_.aimingSec, 1, false);
}

void BossCharge::InitWarning()
{	//アニメーション変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Charging], parameters_.warningSec, 1,false);
	//ダッシュ方向指定して速度加算
	dash_.velo = Vector3{ boss_->GetPlayerWorldTranslate() - boss_->world_.translate_ }.SetNormalize() * dash_.spd;
}

void BossCharge::InitATK()
{
	//攻撃コライダーを有効
	boss_->GetATKCollider().isActive_ = true;
	//アニメーションを変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Charging], parameters_.ATKSec,dash_.animeSpd);
	//アニメーション再生の０-１管理処理をOFF
	boss_->SetAnimeTime(false);

	//座標を保存
	dash_.prePos = boss_->world_.translate_;
	//プレイヤーとの距離を取得
	dash_.length = boss_->GetBoss2PlayerDirection().GetLength();
}

void BossCharge::InitStiffness() {
	//攻撃コライダー無効化
	boss_->GetATKCollider().isActive_ = false;
	//アニメーション変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::EdCharge], parameters_.stiffnessSec, 1, false);
}

void BossCharge::InitBack() {
	//アニメーションん変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], parameters_.backSec, 1, false);
}

void BossCharge::UpdateAIMing()
{
	//プレイヤー方向を見続ける
	Vector3 direc = boss_->GetBoss2PlayerDirection();

	//重なっていない場合
	if (direc != Vector3(0, 0, 0)) {
		//回転Yを計算して与える
		boss_->world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}

	//カウントが達成していた場合
	if (currentCount_ >= parameters_.aimingSec) {
		//次の状態に移行
		behaviReq_ = Warning;
	}
	else {//カウントが未達の場合
		//割合取得
		float t = currentCount_ / parameters_.aimingSec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	}
}

void BossCharge::UpdateWarning()
{
	//カウント達成の場合
	if (currentCount_ >= parameters_.warningSec) {
		//次の状態に移行
		behaviReq_ = ATK;
	}
	else {//カウント未達の場合
		//割合取得
		float t = currentCount_ / parameters_.warningSec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	}
}

void BossCharge::UpdateATK()
{
	//プレイヤー方向にダッシュ
	boss_->world_.translate_ += dash_.velo * (float)DeltaTimer::deltaTime_;

	//過去位置からの距離取得
	float leng = Vector3{ dash_.prePos - boss_->world_.GetWorldTranslate() }.GetLength();

	//最初に指定した距離以上で止まる処理
	if (leng >= dash_.length) {
		//次の状態へ
		behaviReq_ = Stiffness;
	}
}

void BossCharge::UpdateStiffness()
{
	//カウント達成の場合
	if (currentCount_ >= parameters_.stiffnessSec) {
		//次の状態へ
		behaviReq_ = Back;
	}
	else {//カウント未達の場合
		//割合取得
		float t = currentCount_ / parameters_.stiffnessSec;

		//速度を段々下げる
		Vector3 velo = Lerp(dash_.velo, { 0,0,0 }, t);

		//ベクトル方向に移動
		boss_->world_.translate_ += velo * (float)DeltaTimer::deltaTime_;

		//変更した速度を保存
		dash_.velo = velo;

		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	
	}
}

void BossCharge::UpdateBack() {

	//カウント達成した場合
	if (currentCount_ >= parameters_.backSec) {
		//攻撃終了フラグをON
		boss_->isFinishedATK_ = true;
	}
	else {//カウント未達の場合
		//割合取得
		float t = currentCount_ / parameters_.backSec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	}

}
