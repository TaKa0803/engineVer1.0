#pragma once
#include"AL/Boss/ATK/IATK/IATK.h"
#include"AL/Boss/BulletManager/BossBulletManager.h"
//#include"Vector3.h"

class BossShotBullet : public IBossATK {
public:

	BossShotBullet();
	~BossShotBullet() = default;

	//void Init() override;
	//void Update()override;

public://状態のデータ
	void InitAIMing()override {};
	void InitWarning()override;
	void InitATK() override {};
	void InitStiffness()override {};
	void InitBack()override {};

	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override;
	void UpdateBack()  override;

private://変数

	//弾マネージャ
	BossBulletManager* bulletM_=nullptr;

private://パラメータ



	//弾の発射数
	float shotNum_ = 5;

	//発射回数
	float shotCount_ = 0;
};