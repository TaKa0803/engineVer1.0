#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"
#include"Input/Input.h"
#include<iostream>
#include<optional>

class Player;

class PlayerBehaviorManager {

public:

	PlayerBehaviorManager(Player*player);
	~PlayerBehaviorManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initilaize();

	//更新
	void Update();

	//プレイヤーポインタ取得
	Player* GetPlayer() { return player_; };
public:

	//攻撃入力の取得処理まとめ
	bool GetATKInput();
	//回転入力の処理まとめ
	bool GetRollInput();
	//ダッシュ入力処理まとめ
	bool GetDashInput();

private://ポインタ参照物

	Input* input_=nullptr;

	Player* player_ = nullptr;

private://変数

	//状態処理まとめ
	std::vector<std::unique_ptr<IPlayerBehavipr>>behaviors_;

	//状態
	enum class Behavior {
		IDLE,		//通常状態
		ROLLING,    //ローリング 
		ATK,		//攻撃
		HITACTION,	//被攻撃時
		kNumStates	//状態の数
	};

	//現在の状態
	Behavior behavior_=Behavior::IDLE;

	//受け取り
	std::optional<Behavior>behaviorReq_=std::nullopt;
};
