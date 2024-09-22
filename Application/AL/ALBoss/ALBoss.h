#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"AL/Player/ALPlayer.h"

class ALBoss : public GameObject {


public:
	ALBoss();
	~ALBoss();

	void Initilaize();

	void Update();

	void Draw();

private://状態管理

	enum class Behavior {
		IDLE,	//待機状態
		MOVE,	//移動
		ATK,    //攻撃
		_CountBehavior
	};

	//プレイヤーの状態
	Behavior behavior_ = Behavior::IDLE;
	//状態リクエスト
	std::optional<Behavior>behaviorRequest_ = std::nullopt;

	//状態ごとの初期化テーブル
	static void (ALBoss::* BehaviorInitialize[])();

	//状態ごとの更新テーブル
	static void (ALBoss::* BehaviorUpdate[])();

private:
	Input* input_ = nullptr;

#pragma region 影
	std::unique_ptr<InstancingGameObject>shadow;
#pragma endregion
};