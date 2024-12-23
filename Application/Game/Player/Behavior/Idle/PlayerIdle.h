#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"

//プレイヤー通常状態
class PlayerIdle : public PlayerBaseBehavior {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">プレイヤ</param>
	PlayerIdle(Player* player);
	~PlayerIdle() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

};