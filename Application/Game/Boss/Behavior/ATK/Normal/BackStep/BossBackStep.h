#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"


class BossBackStep : public IBossATK {

public:

	//コンストラクタ
	BossBackStep();
	~BossBackStep() = default;


public://状態のデータ
	void InitAIMing()override ;
	void InitWarning()override {};
	void InitATK() override {};
	void InitStiffness() override {};
	void InitBack() override;

	void UpdateAIMing() override ;
	void UpdateWarning() override {};
	void UpdateATK()  override {};
	void UpdateStiffness() override {};
	void UpdateBack()  override;

private://変数

	//速度
	Vector3 velo_{};
	//加速度
	Vector3 acce_{};

	//Back状態でのさらなる状態
	enum class BackBehavior {
		IDLE,
		ATK,
		CountBackBehavior
	}backBehavior_;



private://パラメータ
	//強襲択が増えるまでの割合
	float percentage_ = 0.5f;

	//初速
	float spd_ = 50.0f;

	//重力
	float gravity_ = 10.0f;

	//とぶ高さの強さ
	float height_ = 1.0f;

	//
	float animeEndMultiply_ = 1.0f;
};