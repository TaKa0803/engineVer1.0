#pragma once
#include"Vector3.h"

class Player;

class PlayerDown {

public:

	PlayerDown(Player* player);
	~PlayerDown() = default;

	void Initialize();

	void Update();

private:

	Player* player_;

	//吹きとび向き
	Vector3 velo_;

private:

	float spd_ = 10;

	float height_ = 2;

	float gravity_ = 10.0f;

};