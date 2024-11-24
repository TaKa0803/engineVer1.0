#pragma once
#include"Vector3.h"

#include"Game/Player/Behavior/IPlayerBehavior.h"

class PlayerDown :public IPlayerBehavior{

public:

	PlayerDown(Player* player);
	~PlayerDown() = default;

	void Initialize()override;

	void Update()override;

private:
	//吹きとび向き
	Vector3 velo_{};

	enum class Behavior {
		Flying,
		landing,
		kNumBehavior
	};

	//状態
	Behavior behavior_ = Behavior::Flying;
private://パラメータ

	float spd_ = 10;

	float height_ = 2;

	float gravity_ = 10.0f;

	float changeAnimeSec_ = 0.1f;

	float loopSec_ = 1.0f;

	float randSec_ = 1.0f;
	float currentLand_=0.0f;

	float returnAnimeSec_ = 1.0;
};