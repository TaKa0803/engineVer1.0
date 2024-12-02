#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"

class Player;

class PlayerHit :public IPlayerBehavior{

public:

	PlayerHit(Player* player);
	~PlayerHit()=default;

	void Initialize()override;

	void Update() override;


private:

	//ヒット時の速度減衰
	float multiplySpd_ = 0.5f;

	//この状態の続く時間
	float sec_ = 1.0f;
	float current_ = 0;

private:


	GVariTree tree_ = GVariTree("ヒット");


	Player* player_;


};