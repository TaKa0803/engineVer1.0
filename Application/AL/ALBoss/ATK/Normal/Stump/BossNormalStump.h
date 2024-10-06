#pragma once
#include"Vector3.h"
#include<optional>

class ALBoss;

//ボスの棒立ちor試案処理
class BossNormalStump {
public:

	BossNormalStump(ALBoss* boss);
	~BossNormalStump() = default;

	void Initialize();

	void Update();

private://**参照物

	ALBoss* boss_;

private://動きの処理

	enum Behavior {
		AIMing,		//浮遊してプレイヤーの真上に行き攻撃を狙う
		Warning,	//警告
		Stump,		//実際の攻撃
		Stiffness,	//攻撃による硬直
		Back,		//別の攻撃移行または
		CountOfATKData
	};

	Behavior behavior_ = AIMing;
	std::optional<Behavior>behaviReq_ = std::nullopt;

	static void (BossNormalStump::* behaviorInit[])();
	static void (BossNormalStump::* behaviorUpdate[])();

	void InitAIMing();
	void InitWarning();
	void InitStump();
	void InitStiffness();
	void InitBack();

	void UpdateAIMing();
	void UpdateWarning();
	void UpdateStump();
	void UpdateStiffness();
	void UpdateBack();

private://**変数

	struct AIMData {
		//開始時の座標
		Vector3 stPos{};

		//どれくらいの高さまで上がるか
		float height = 10;

		//次の状態に行くまでの時間
		float maxGoTop = 3.0f;


		//高さがどれくらい早く到着するか
		float goTopSec = 1.0f;
		//XZがどれくらい早く合うか
		float xzSetSec = 2.0f;
	};

	struct  WarningData {
		float maxWarning = 1.0f;//何秒後に落ちるか
	};

	struct StumpData
	{
		Vector3 velo{ 0,-1,0 };
		float spd = 80;
	};

	struct StiffnessData
	{
		float stifnessSec = 5.0f;
	};

	struct BackData
	{
		float maxBackSec = 1.0f;
	};

	struct Data
	{
		AIMData aim;
		WarningData warning;
		StumpData stump;
		StiffnessData stiffnrss;
		BackData back;

		float currentCount = 0.0f;

	};

	Data data_;

public:
	//終了チェックフラグ
	bool isFinished_ = false;
};