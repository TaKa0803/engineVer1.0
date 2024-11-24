#pragma once
#include"Game/Player/Behavior/ATKData/PlayerPunch.h"
#include"GvariGroup/GvariGroup.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"
#include<iostream>

class Player;

class PlayerATKManager :public IPlayerBehavior{

public:
	PlayerATKManager(Player* player);
	~PlayerATKManager() = default;

	void Initialize()override;

	void Update()override;

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



private://デバッグ関係


	std::string typeName_[CountATKType ] = {
		"パンチ"
	};

	std::string nowType_ = "";
};