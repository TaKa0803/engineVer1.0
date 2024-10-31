#pragma once
#include"Vector3.h"

class ALPlayer;

class PlayerDown {

public:

	PlayerDown(ALPlayer* player);
	~PlayerDown() = default;

	void Initialize();

	void Update();

private:

	ALPlayer* player_;

	//吹きとび向き
	Vector3 velo_;

private:

	float spd_ = 10;

	float height_ = 2;

	float gravity_ = 10.0f;

};