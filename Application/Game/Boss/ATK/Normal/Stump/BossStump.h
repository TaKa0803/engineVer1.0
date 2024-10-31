#pragma once
#include"Game/Boss/ATK/IATK/IATK.h"
#include"Vector3.h"

class BossStump : public IBossATK {
public:
	BossStump();
	~BossStump() = default;

public://状態のデータ
	void InitAIMing()override;		//溜め
	void InitWarning()override;		//ジャンプ
	void InitATK() override;		//落下
	void InitStiffness() override;	//硬直
	void InitBack()override;		//復帰

	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override;
	void UpdateBack()  override;

private:

	struct AIMData {
		//次の状態に行くまでの時間
		float maxGoTop = 3.0f;
	};

	struct  WarningData {
		float maxWarning = 1.0f;//何秒後に落ちるか

		//最高高度
		float height = 15.0f;

		//この距離感でプレイヤーに落下
		float length = 5.0f;

		//パラメータ
		//開始位置
		Vector3 stPos;

	};

	struct StumpData
	{
		Vector3 velo{ 0,-1,0 };
		float spd = 60;
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
		//ターゲット座標
		Vector3 stPos{};

		AIMData aim;
		WarningData warning;
		StumpData stump;
		StiffnessData stiffnrss;
		BackData back;
	}data_{};
};