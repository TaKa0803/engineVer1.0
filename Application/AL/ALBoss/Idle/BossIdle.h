#pragma once

class ALBoss;

//ボスの棒立ちor試案処理
class BossIdle {
public:

	BossIdle(ALBoss* boss);
	~BossIdle() = default;

	void Initialize();

	void Update();

private:

	ALBoss* boss_;

	struct Data
	{
		//待機時間
		float idleSec = 3.0f;
		float currentSec = 0.0f;

	};

	Data data_;
};