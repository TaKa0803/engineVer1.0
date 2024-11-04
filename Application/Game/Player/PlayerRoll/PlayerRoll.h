#pragma once
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"GvariGroup/GvariGroup.h"

//前方宣言
class Player;

class PlayerRoll {

public:
	PlayerRoll(Player* player);
	~PlayerRoll() = default;

	void Initialize();

	void Update();

	void Debug();

	GVariTree& GetTree() { return tree_; }
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
	//プレイヤー参照ポインタ
	Player* player_;

	//入力
	Input* inp_;

	//カメラ
	Camera* camera_;

private://デバッグ
	GVariTree tree_ = GVariTree("roll");

};