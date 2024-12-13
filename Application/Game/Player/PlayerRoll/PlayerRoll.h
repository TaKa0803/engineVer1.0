#pragma once
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"GvariGroup/GvariGroup.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"

//前方宣言
class Player;

class PlayerRoll :public PlayerBaseBehavior{

public:
	//コンストラクタ
	PlayerRoll(Player* player);
	~PlayerRoll() = default;

	//初期化
	void Initialize()override;

	//更新
	void Update()override;


public:
	//データ
	struct Data {

		//移動速度
		float spd=40.0f;

		//止まるまでの時間
		float stopSec = 0.5f;

		//止まるまでのカウント
		float currentStop = 0;

		//移動するする秒数
		float rollSec=1.0f;
		//移動時間時間カウント
		float currentRoll = 0;
	};

	//パラメータ群
	Data data_{};
private:

	//入力
	Input* inp_;

	//カメラ
	Camera* camera_;

};