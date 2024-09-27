#pragma once
#include"Vector3.h"
#include<optional>

class ALBoss;

struct BulletData
{
	Vector3 pos;
};

class ShotBullet {

public:
	ShotBullet(ALBoss* boss);
	~ShotBullet();

	void Init();

	void Update();

	ALBoss* boss_;

private://動きの処理

	enum Behavior {
		AIMing,		//浮遊してプレイヤーの真上に行き攻撃を狙う
		Warning,	//警告
		Stump,		//実際の攻撃
		Stiffness,	//攻撃による硬直
		Back,		//別の攻撃移行または
		_countOfATKData
	};

	Behavior behavior_ = AIMing;
	std::optional<Behavior>behaviReq_ = std::nullopt;

	static void (ShotBullet::* behaviorInit[])();
	static void (ShotBullet::* behaviorUpdate[])();
};