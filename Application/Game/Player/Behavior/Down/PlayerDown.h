#pragma once
#include"Vector3.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"

class PlayerDown : public IPlayerBehavipr{

public:

	PlayerDown(PlayerBehaviorManager* player);
	~PlayerDown() = default;

	void Initialize()override;

	void Update()override;

private:


	//吹きとび向き
	Vector3 velo_;

private:

	float spd_ = 10;

	float height_ = 2;

	float gravity_ = 10.0f;

};