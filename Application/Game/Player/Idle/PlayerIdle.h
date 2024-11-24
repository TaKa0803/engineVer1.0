#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"

class PlayerIdle : public IPlayerBehavior {

public:
	PlayerIdle(Player* player);
	~PlayerIdle() = default;

	void Initialize()override;

	void Update() override;

};