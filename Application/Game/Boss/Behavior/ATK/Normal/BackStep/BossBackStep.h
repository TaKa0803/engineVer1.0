#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"

//ボスのバックステップ処理
class BossBackStep : public BossBaseATK {

public://**パブリック関数**//

	//コンストラクタ
	BossBackStep();
	~BossBackStep() = default;

public: //**各状態の処理関数**//

	//非利用の部分はoverride後{}で記述

	//各状態初期化処理
	void InitAIMing()override;
	void InitWarning()override {};
	void InitATK() override {};
	void InitStiffness() override {};
	void InitBack() override;

	//各状態更新処理
	void UpdateAIMing() override ;
	void UpdateWarning() override {};
	void UpdateATK()  override {};
	void UpdateStiffness() override {};
	void UpdateBack()  override {};

private: //**変数**//

	//速度
	Vector3 velo_{};
	//加速度
	Vector3 acce_{};

	//Back状態でのさらなる状態
	enum class BackBehavior {
		IDLE,				//待機
		ATK,				//攻撃
		CountBackBehavior	//さらなる状態の数
	}backBehavior_=BackBehavior::IDLE;

	//攻撃番号の実際の攻撃
	enum ATKNum {
		Stump,			//落下攻撃
		Shot,			//弾発射
		CountATKNum
	};

private://パラメータ



	//強襲択が増えるまでの割合
	float percentage_ = 0.5f;

	//初速
	float spd_ = 50.0f;

	//重力
	float gravity_ = 10.0f;

	//とぶ高さの強さ
	float height_ = 1.0f;

	//アニメーション再生倍率
	float animeEndMultiply_ = 1.0f;
};