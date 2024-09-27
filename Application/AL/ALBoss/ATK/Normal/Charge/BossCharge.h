#pragma once
#include"Vector3.h"
#include<optional>

class ALBoss;

class BossCharge {
public:
	BossCharge(ALBoss* boss);
	~BossCharge()=default;

	void Initialize();

	void Update();

private:
	ALBoss* boss_;


private://**状態管理
	enum Behavior {
		AIMing,		//浮遊してプレイヤーの真上に行き攻撃を狙う
		Warning,	//警告
		Dash,		//実際の攻撃
		Stiffness,	//攻撃による硬直
		Back,		//別の攻撃移行または
		_countOfATKData
	};

	Behavior behavior_ = AIMing;
	std::optional<Behavior>behaviReq_ = std::nullopt;

	static void (BossCharge::* behaviorInit[])();
	static void (BossCharge::* behaviorUpdate[])();

	void InitAIMing();
	void InitWarning();
	void InitDash();
	void InitStiffness();
	void InitBack();

	void UpdateAIMing();
	void UpdateWarning();
	void UpdateDash();
	void UpdateStiffness();
	void UpdateBack();


private://**パラメータ

	struct AIMData
	{
		float maxSec = 1.0f;
	};
	struct WarningData
	{
		float maxSec = 1.0f;
	};
	struct DashData
	{
		Vector3 pos;
		float spd = 30.0f;
	};
	struct StiffnessData {
		float sec = 5.0f;
	};
	struct BackData {
		float sec = 1.0f;
	};

	struct ATKData
	{
		AIMData aim;
		WarningData warning;
		DashData dash;
		StiffnessData stiffness;
		BackData back;

		float currentCount = 0;
	};

};