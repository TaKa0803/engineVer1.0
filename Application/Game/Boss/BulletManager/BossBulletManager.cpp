#include "BossBulletManager.h"
#include"DeltaTimer/DeltaTimer.h"
#include"TextureManager/TextureManager.h"

BossBulletManager::BossBulletManager()
{
	//ヒットエフェクト生成
	hiteffect_ = std::make_unique<EffectNormal>("smallDebris", "hitplaneEffect");

	//あらかじめメモリを確保しておく
	datas_.reserve(10);
	InstancingGameObject::Initialize("BossBullet");
	IMM_->SetTexture(tag_, TextureManager::LoadTexShortPath("AL/whiteGrid.png"));

	//デバッグ用にツリーにパラメータを設定
	tree_.name_ = "弾マネージャ";
	tree_.SetValue("弾の速度", &spd_);
	tree_.SetValue("死亡までのカウント", &maxDeadSec_);
	tree_.SetValue("弾のサイズ", &ammoScale_);
	tree_.SetValue("コライダーサイズ", &colliderScale_);
	tree_.SetTreeData(IMM_->CreateAndGetTree(tag_, "モデル"));
}

void BossBulletManager::Initialize()
{
	//データを初期化
	datas_.clear();
	//ヒットエフェクト初期化
	hiteffect_->Initialize();
}

void BossBulletManager::Update()
{
	//進んでワールド更新
	for (auto& d : datas_) {

		//死亡チェック
		if (d->deadCount >= maxDeadSec_||d->ishit) {
			d->ishit = true;
		}
		else {
			//座標更新
			d->data.world.translate_ += d->data.velo * (float)DeltaTimer::deltaTime_;
			d->data.world.UpdateMatrix();
			//コライダー更新
			d->collider->Update();

			//死亡カウント増加
			d->deadCount += (float)DeltaTimer::deltaTime_;

			//地面めり込みで削除
			if (d->data.world.GetWorldTranslate().y <= 0) {
				//ヒットフラグON
				d->ishit = true;
				//ヒットエフェクト生成
				hiteffect_->SpawnEffect(d->data.world.GetWorldTranslate());
			}
		}
	}

	//死亡済データ削除
	datas_.erase(std::remove_if(datas_.begin(), datas_.end(),
		[&](std::unique_ptr<Datas>& data) {
			//ヒット判定フラグONで削除
			if (data->ishit) {
				return true; 
			};
			//未ヒットなので消さない
			return false; }),
		datas_.end());

	//ヒットエフェクト更新
	hiteffect_->Update();
}

void BossBulletManager::Draw()
{
	//弾の描画
	for (auto& d : datas_) {
		//データを送信
		IMM_->SetData(tag_, d->data.world);
		//コライダー描画
		d->collider->Draw();
	}
	//ヒットエフェクトを描画
	hiteffect_->Draw();
}

void BossBulletManager::SetData(const BossBulletData& data)
{
	//新しいデータポインタ生成
	std::unique_ptr<Datas> newd = std::make_unique<Datas>();
	//基本データ格納
	newd->data = data;
	//サイズ設定
	newd->data.world.SetScale(ammoScale_);
	//速度設定
	newd->data.velo.SetNormalize();
	newd->data.velo *= spd_;
	//コライダー初期化
	newd->collider = std::make_unique<SphereCollider>();
	newd->collider->Initialize("bossB",newd->data.world);
	newd->collider->SetRadius(colliderScale_);
	//データセット
	datas_.push_back(std::move(newd));

}

bool BossBulletManager::CheckCollision(const SphereCollider* co)
{
	//一回でもあたったのフラグ
	bool ans = false;

	//押し返し量（使わない
	Vector3 back;

	//すべての弾チェック
	for (auto& d : datas_) {
		//当たり判定処理
		if (!d->ishit&&d->collider->IsCollision(co, back)) {
			//ヒット判定とコライダーヒット判定有効
			ans =true;
			d->ishit = true;
		}

	}

	//返却
	return ans;
}
