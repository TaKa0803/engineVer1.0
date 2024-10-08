#pragma once

class Boss;

//様子見移動
class BossMove {
public:

	BossMove(Boss* boss);
	~BossMove() = default;

	void Initialize();

	void Update();

private:

	Boss* boss_;

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