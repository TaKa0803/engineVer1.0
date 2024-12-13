#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"

//ボスのパンチ攻撃
class BossPunch : public BossBaseATK {

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BossPunch();
	~BossPunch()=default;

public: //**各状態の処理関数**//

	//各状態の初期化
	void InitAIMing()override;		//溜め
	void InitWarning()override {};		//ジャンプ
	void InitATK() override;		//落下
	void InitStiffness() override {};	//硬直
	void InitBack()override;		//復帰
	 
	//各状態の更新
	void UpdateAIMing() override;
	void UpdateWarning() override {};
	void UpdateATK()  override;
	void UpdateStiffness() override {};
	void UpdateBack()  override;

};