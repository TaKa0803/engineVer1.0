#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"

//前方宣言
class Player;

class PlayerIdle : public IPlayerBehavipr {

public:

	PlayerIdle(Player*player);
	~PlayerIdle()=default;

	void Initialize();

	void Update();
};