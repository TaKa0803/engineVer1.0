#pragma once
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"




class PlayerRoll : public  IPlayerBehavipr{

public:
	PlayerRoll(PlayerBehaviorManager* player);
	~PlayerRoll() = default;

	void Initialize()override;

	void Update() override;

public:
	//データ
	struct Data {

		Vector3 stPos{};

		float spd=40.0f;
		float length = 20.0f;

		float stopSec = 0.5f;

		float currentStop = 0;
	};

	Data data_{};
private:

	//入力
	Input* inp_;

	//カメラ
	Camera* camera_;

private://デバッグ
	//GVariTree tree_ = GVariTree("roll");

};