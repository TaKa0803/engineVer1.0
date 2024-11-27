#pragma once
#include"Game/Boss/ATK/IATK/IATK.h"

class BossPunch : public IBossATK {

public:

	BossPunch();
	~BossPunch()=default;

public:

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

};