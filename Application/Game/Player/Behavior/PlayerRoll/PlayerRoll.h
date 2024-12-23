#pragma once
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"GlobalVariable/Tree/GlobalVariableTree.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"

//前方宣言
class Player;

//プレイヤー回避行動
class PlayerRoll :public PlayerBaseBehavior{

public://**パブリック変数**//

	//データ
	struct Data {

		//移動速度
		float spd = 40.0f;

		//止まるまでの時間
		float stopSec = 0.5f;

		//止まるまでのカウント
		float currentStop = 0;

		//移動するする秒数
		float rollSec = 1.0f;
		//移動時間時間カウント
		float currentRoll = 0;
	}data_{};

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">プレイヤーポインタ</param>
	PlayerRoll(Player* player);
	~PlayerRoll() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

private://**プライベート変数**//

	//入力
	Input* inp_;

	//カメラ
	Camera* camera_;

};