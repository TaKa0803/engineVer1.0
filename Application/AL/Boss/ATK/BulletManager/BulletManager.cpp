#include "BulletManager.h"


BulletManager::BulletManager(Boss* boss)
{
	boss;

	datas_.reserve(10);
}

void BulletManager::Init()
{
	InstancingGameObject::Initialize("bossBullet");
}

void BulletManager::Update()
{
	for (auto& d : datas_) {
		d.data.pos.translate_ += d.data.velo;
		d.data.pos.UpdateMatrix();

		d.collider->Update();
	}

	datas_.erase(std::remove_if(datas_.begin(), datas_.end(),
		[](Datas& data) {if (data.ishit) { return true; }; return false; }),
		datas_.end());
}

void BulletManager::Draw(int anime)
{

	for (auto& d : datas_) {
		IMM_->SetData(tag_, d.data.pos);

		d.collider->Draw();
	}
}

void BulletManager::SetData(const BossBulletData& data)
{

	Datas newd;
	newd.data = data;
	newd.collider = std::make_unique<SphereCollider>();
	newd.collider->Initialize("bossB",newd.data.pos);
	newd.collider->SetRadius(1.0f);

	datas_.emplace_back(std::move(newd));

}

bool BulletManager::CheckCollision(const SphereCollider* co)
{
	bool ans = false;
	Vector3 back;
	for (auto& d : datas_) {

		//当たり判定処理
		if (!d.ishit&&d.collider->IsCollision(co, back)) {
			ans =true;
			d.ishit = true;
		}

	}

	return ans;
}
