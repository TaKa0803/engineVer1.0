#pragma once
#include"Game/Boss/ATK/IATK/IATK.h"

class BossSumerSolt : public IBossATK {
public:
	BossSumerSolt();
	~BossSumerSolt()=default;

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



};