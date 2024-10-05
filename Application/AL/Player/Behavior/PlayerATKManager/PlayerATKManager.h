#pragma once

class ALPlayer;

class PlayerATKManager {

public:
	PlayerATKManager(ALPlayer* player);
	~PlayerATKManager() = default;

	void Initialize();

	void Update();

public://パブリック変数

	//終了取得
	bool isEnd_ = false;

	//攻撃のタイプ指定
	enum ATKType {
		Punch,
		_countATKType
	};

	//攻撃タイプ
	ATKType type_ = Punch;
private://各状態まとめ

	static void (PlayerATKManager::* TypeInit[])();
	static void (PlayerATKManager::* TypeUpdate[])();

	void InitPunch();
	void UpdatePunch();

private://プライベート変数

	//プレイヤーのポインタ
	ALPlayer* player_;

};