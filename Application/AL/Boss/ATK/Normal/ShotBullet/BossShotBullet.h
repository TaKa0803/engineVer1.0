#pragma once
#include"AL/Boss/ATK/IATK/IATK.h"
//#include"Vector3.h"

class BossShotBullet : public IATK {
public:

	BossShotBullet();
	~BossShotBullet() = default;

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



};