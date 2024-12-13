#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"
#include"Vector3.h"

//ボスの落下攻撃処理
class BossStump : public BossBaseATK {
public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BossStump();
	~BossStump() = default;

public: //**各状態の処理関数**//

	//各状態初期化処理
	void InitAIMing()override;		//溜め
	void InitWarning()override;		//ジャンプ
	void InitATK() override;		//落下
	void InitStiffness() override;	//硬直
	void InitBack()override;		//復帰

	//各状態更新処理
	void UpdateAIMing() override;
	void UpdateWarning() override;
	void UpdateATK()  override;
	void UpdateStiffness() override;
	void UpdateBack()  override;

private: //**パラメータ**//

	//ジャンプ中パラメータ群
	struct  WarningData {
		//最高高度
		float height = 15.0f;

		//この距離感でプレイヤーに落下
		float length = 5.0f;

		//開始位置
		Vector3 stPos;
	};

	//落下中パラメータ群
	struct StumpData
	{
		//落下向きベクトル
		Vector3 velo{ 0,-1,0 };
		//落下速度
		float spd = 60;
		//アニメの再生倍率
		float animeMultiply = 2.0f;
	};

	//パラメータ群
	struct Data
	{
		//ターゲット座標
		Vector3 stPos{};

		//飛翔時のパラメータ
		WarningData warning;
		//落下時のパラメータ
		StumpData stump;

	}data_{};
};