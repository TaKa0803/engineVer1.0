#pragma once
#include"Game/Player/Behavior/ATKData/PlayerPunch.h"
#include"GvariGroup/GvariGroup.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"
#include<iostream>

//前方宣言
class Player;

//プレイヤー攻撃マネージャ
class PlayerATKManager :public PlayerBaseBehavior{

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player"></param>
	PlayerATKManager(Player* player);
	~PlayerATKManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	
	/// <summary>
	/// 更新処理
	/// </summary>
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

	//状態の初期化の関数ポインタ
	static void (PlayerATKManager::* TypeInit[])();
	//状態の更新の関数ポインタ
	static void (PlayerATKManager::* TypeUpdate[])();

	/// <summary>
	/// パンチ初期化
	/// </summary>
	void InitPunch();

	/// <summary>
	/// パンチ更新
	/// </summary>
	void UpdatePunch();

	//パンチ攻撃の処理クラス
	std::unique_ptr<PlayerPunch> punch_;

private://デバッグ関係


	std::string typeName_[CountATKType ] = {
		"パンチ"
	};

	std::string nowType_ = "";
};