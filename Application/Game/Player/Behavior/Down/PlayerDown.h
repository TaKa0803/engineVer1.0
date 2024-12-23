#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"

//ダウンする処理
class PlayerDown :public PlayerBaseBehavior {

public:	//**パブック関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">プレイヤー</param>
	PlayerDown(Player* player);
	~PlayerDown() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

private: //**プライベート変数**//

	//吹きとび向き
	Vector3 velo_{};

	enum class Behavior {
		Flying,
		landing,
		kNumBehavior
	};

	//状態
	Behavior behavior_ = Behavior::Flying;

private://パラメータ

	//速度
	float spd_ = 10;

	//傾き量
	float height_ = 2;

	//重力
	float gravity_ = 10.0f;

	//アニメ変更時間
	float changeAnimeSec_ = 0.1f;

	//ループ時間
	float loopSec_ = 1.0f;

	//ランダム時間
	float randSec_ = 1.0f;

	//カウント
	float currentLand_ = 0.0f;

	//戻りアニメ時間
	float returnAnimeSec_ = 1.0;
	
	//復帰時移動速度倍率
	float moveSpdMuli_ = 0.5f;
};