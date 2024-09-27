#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"AL/Player/ALPlayer.h"
#include"AL/CirccleShadow/CirccleShadow.h"
#include"SphereCollider/SphereCollider.h"

#include"AL/ALBoss/Idle/BossIdle.h"
#include"AL/ALBoss/Move/BossMove.h"
#include"AL/ALBoss/ATK/BossATK.h"

class ALBoss : public GameObject {


public:
	ALBoss(ALPlayer* player);
	~ALBoss();

	void Initilaize();

	void Update();

	void Draw();

	//攻撃ヒット時の処理
	void OnCollision();

	SphereCollider& GetBodyCollider()& { return *collider_.get(); }

	//プレイヤー方向への向きベクトルを取得
	Vector3 GetBoss2PlayerDirection();

	Vector3 GetPlayerWorldTranslate() { return player_->GetWorld().GetWorldTranslate(); };
public://状態管理
#pragma region 状態

	enum Behavior {
		IDLE,	//待機状態
		MOVE,	//移動
		ATK,    //攻撃
		_CountBehavior
	};

	//プレイヤーの状態
	Behavior behavior_ = Behavior::IDLE;
	//状態リクエスト
	std::optional<Behavior>behaviorReq_ = std::nullopt;

	bool isFinishedATK_ = false;
private://状態管理

	//状態ごとの初期化テーブル
	static void (ALBoss::* BehaviorInitialize[])();

	//状態ごとの更新テーブル
	static void (ALBoss::* BehaviorUpdate[])();

#pragma region 各状態
	void InitIdle();
	void InitMove();
	void InitATK();

	void UpdateIdle();
	void UpdateMove();
	void UpdateATK();
#pragma endregion

#pragma endregion
private://**参照
	Input* input_ = nullptr;

	ALPlayer*player_;

private://**変数

	std::unique_ptr<CirccleShadow>shadow_;

	std::unique_ptr<SphereCollider> collider_;

	//各処理
	std::unique_ptr<BossIdle>idle_;
	std::unique_ptr<BossMove>move_;
	std::unique_ptr<BossATK>atk_;
	

	std::unique_ptr<SphereCollider>atkCollider_;

public://**パラメータ

	int HP_ = 50;

private://ImGui用

	//
	std::string behaviorName_[(int)Behavior::_CountBehavior] = {
		"Idle",
		"Move",
		"ATK"
	};
};