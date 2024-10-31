#pragma once
#include"Input/Input.h"
#include"Camera/Camera.h"
//前方宣言
class ALPlayer;

class PlayerRoll {

public:
	PlayerRoll(ALPlayer* player);
	~PlayerRoll() = default;

	void Initialize();

	void Update();

	void Debug();
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
	ALPlayer* player_;

	//入力
	Input* inp_;

	//カメラ
	Camera* camera_;
};