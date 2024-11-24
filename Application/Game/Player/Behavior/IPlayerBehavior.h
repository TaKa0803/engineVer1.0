#pragma once
#include"GvariGroup/GvariGroup.h"

class Player;

class IPlayerBehavior {

public:

	virtual void Initialize() = 0;
	virtual void Update() = 0;

	GVariTree& GetTree() { return tree_; }
public:

	Player* player_;

	GVariTree tree_ = GVariTree("");
};