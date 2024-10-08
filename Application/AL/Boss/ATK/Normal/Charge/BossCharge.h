#pragma once
#include"AL/Boss/ATK/IATK/IATK.h"
#include"Vector3.h"

class BossCharge1 : public IATK {
public:

	BossCharge1()=default;

	//void Init() override;
	//void Update()override;

public://状態のデータ
	void InitAIMing()override;
	void InitWarning()override;
	void InitATK() override;
	void InitStiffness() override;
	void InitBack() override;

	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override;
	void UpdateBack()  override;

private://変数

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
		Vector3 prePos;
		Vector3 velo;
		float length = 0;
		float spd = 30.0f;
	};
	struct StiffnessData {
		float sec = 0.5f;
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

	}data_;
};