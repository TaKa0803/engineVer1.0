#pragma once
#include"AL/ALBoss/ATK/Normal/Stump/BossNormalStump.h"
#include"AL/ALBoss/ATK/Normal/Charge/BossCharge.h"

#include<iostream>

class ALBoss;

class BossNormal {

public:
	BossNormal(ALBoss*boss);
	~BossNormal() = default;

	void Initialize();

	void Update();

private://参照してきたもの

	ALBoss* boss_;

private://**ノーマルの攻撃手段
	enum ATKType {
		Stump,			//潰す攻撃
		ShotBullet,		//弾発射
		Charge,			//突撃
		_countOfATKData
	};

	ATKType type_=Stump;

	static void (BossNormal::* typeInit[])();
	static void (BossNormal::* typeUpdate[])();

	void InitStump();
	void UpdateStump();

	void InitShotBullet();
	void UpdateShotBullet();

	void InitCharge();
	void UpdateCharge();

	std::unique_ptr<BossNormalStump>stump_;
	std::unique_ptr<BossCharge>charge_;


public:
	bool isFinished_ = false;
};