#pragma once
#include"Game/Boss/IBossBehavior.h"

//ボスの棒立ちor試案処理
class BossIdle :public IBossBehavior{
public:

	BossIdle(Boss* boss);
	~BossIdle() = default;

	void Initialize()override;

	void Update()override;

private:

	Boss* boss_;

	struct Data
	{
		//待機時間
		float idleSec = 3.0f;
		float currentSec = 0.0f;

	};

	Data data_;

private://ImGui関係
};