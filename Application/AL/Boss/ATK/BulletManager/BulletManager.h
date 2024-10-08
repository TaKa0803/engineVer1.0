#pragma once
#include"InstancingGameObject/InstancingGameObject.h"
#include"SphereCollider/SphereCollider.h"
#include<vector>

struct BossBulletData {
	EulerWorldTransform pos;
	Vector3 velo;
};

class Boss;
class BulletManager :public InstancingGameObject{
public:

	BulletManager(Boss* boss);
	~BulletManager() = default;

	void Init();
	void Update()override;
	void Draw(int anime =0)override;

	void SetData(const BossBulletData&data);

	bool CheckCollision(const SphereCollider* co);
private:

	struct Datas {
		BossBulletData data;

		std::unique_ptr<SphereCollider> collider;

		bool ishit = false;
	};

	std::vector<Datas>datas_;
};