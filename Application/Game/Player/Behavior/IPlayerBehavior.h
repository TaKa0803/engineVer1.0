#pragma once
#include"GvariGroup/GvariGroup.h"

class PlayerBehaviorManager;

class IPlayerBehavipr {
public:

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	GVariTree& GetTree() { return tree; };


	PlayerBehaviorManager* playerBehaviorM_;


public://ImGui関係

	GVariTree tree = GVariTree("");


};