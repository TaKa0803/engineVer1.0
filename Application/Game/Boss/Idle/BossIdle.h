#pragma once
#include"GvariGroup/GvariGroup.h"

class Boss;

//ボスの棒立ちor試案処理
class BossIdle {
public:

	BossIdle(Boss* boss);
	~BossIdle() = default;

	void Initialize();

	void Update();

	GVariTree& GetTree() { return tree; };
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
	GVariTree tree = GVariTree("Idle");
};