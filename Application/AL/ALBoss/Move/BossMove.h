#pragma once

class ALBoss;

//様子見移動
class BossMove {
public:

	BossMove(ALBoss* boss);
	~BossMove() = default;

	void Initialize();

	void Update();

private:

	ALBoss* boss_;

	struct Data
	{
		//行動時間
		float moveSec = 2.0f;
		float currentSec = 0.0f;

		//速度
		float spdSec = 3.0f;

		//移動方向
		float moveV = 0.0f;
	};

	Data data_;
};