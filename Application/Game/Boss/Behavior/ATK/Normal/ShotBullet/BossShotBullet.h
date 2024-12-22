#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"

//ボスの弾発射攻撃処理
class BossShotBullet : public BossBaseATK {
public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BossShotBullet();
	~BossShotBullet() = default;

public: //**各状態の処理関数**//

	//各状態の初期化
	void InitAIMing()override;
	void InitWarning()override;
	void InitATK() override {};
	void InitStiffness()override {};
	void InitBack()override;

	//各状態の更新
	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override {};
	void UpdateBack()  override;

private: //**変数**//

	//手のボーン名
	//右
	const std::string rHandBoneName_ = "RH";
	//左
	const std::string lHandBoneName_ = "LH";

private: //**パラメータ**//

	//弾の発射数
	float shotNum_ = 5;

	//発射回数
	float shotCount_ = 0;
};