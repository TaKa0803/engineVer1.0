#pragma once
#include"Game/Player/Behavior/ATKData/PlayerPunch.h"
#include"GvariGroup/GvariGroup.h"
#include<iostream>

class Player;

class PlayerATKManager {

public:
	PlayerATKManager(Player* player);
	~PlayerATKManager() = default;

	void Initialize();

	void Update();

public://パブリック変数

	//終了取得
	bool isEnd_ = false;

	//攻撃のタイプ指定
	enum ATKType {
		Punch,
		CountATKType
	};

	//攻撃タイプ
	ATKType type_ = Punch;
private://各状態まとめ

	static void (PlayerATKManager::* TypeInit[])();
	static void (PlayerATKManager::* TypeUpdate[])();

	void InitPunch();
	void UpdatePunch();

	std::unique_ptr<PlayerPunch> punch_;

private://プライベート変数

	//プレイヤーのポインタ
	Player* player_;

private://デバッグ関係
	GVariTree tree_;


	std::string typeName_[CountATKType ] = {
		"パンチ"
	};

	std::string nowType_ = "";
};