#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"
#include"Vector3.h"

//タックル攻撃
class BossCharge : public BossBaseATK {
public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BossCharge();
	~BossCharge() = default;

public: //**各状態の処理関数**//

	//各状態の初期化
	void InitAIMing()override;
	void InitWarning()override;
	void InitATK() override;
	void InitStiffness() override;
	void InitBack() override ;

	//各状態の更新
	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override;
	void UpdateBack()  override ;

private: //**プライベート変数**//

	//ダッシュ時の変数とパラメータ群
	struct DashData
	{
		Vector3 prePos;		//開始時の座標
		Vector3 velo;		//移動ベクトル
		float length = 0;	//距離
		float spd = 40.0f;	//速度
		float animeSpd = 1.0f;	//アニメーション速度
	}dash_;

};