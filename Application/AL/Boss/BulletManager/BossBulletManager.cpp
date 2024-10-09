#include "BossBulletManager.h"


BossBulletManager* BossBulletManager::GetInstance()
{
	static BossBulletManager ins;
	return &ins;
}



void BossBulletManager::SetUp()
{
	datas_.reserve(10);
	InstancingGameObject::Initialize("bossBullet");
}

void BossBulletManager::Init()
{

	datas_.clear();

}

void BossBulletManager::Update()
{

	//進んでワールド更新
	for (auto& d : datas_) {
		d->data.world.translate_ += d->data.velo;
		d->data.world.UpdateMatrix();

		d->collider->Update();
	}

	datas_.erase(std::remove_if(datas_.begin(), datas_.end(),
		[&](std::unique_ptr<Datas>& data) {if (data->ishit) { return true; }; return false; }),
		datas_.end());
}

void BossBulletManager::Draw(int anime)
{

	for (auto& d : datas_) {
		IMM_->SetData(tag_, d->data.world);

		d->collider->Draw();
	}
}

void BossBulletManager::SetData(const BossBulletData& data)
{

	std::unique_ptr<Datas> newd = std::make_unique<Datas>();
	newd->data = data;
	newd->collider = std::make_unique<SphereCollider>();
	newd->collider->Initialize("bossB",newd->data.world);
	newd->collider->SetRadius(1.0f);

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
