#pragma once
#include"AL/Boss/ATK/IATK/IATK.h"
#include"Vector3.h"

class BossStump : public IBossATK {
public:
	BossStump() = default;
	~BossStump() = default;

public://状態のデータ
	void InitAIMing()override;
	void InitWarning()override {};
	void InitATK() override {};
	void InitStiffness() override {};
	void InitBack()override{};

	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override;
	void UpdateBack()  override;

private:

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


	}data_{};
};