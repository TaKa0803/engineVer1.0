#pragma once
#include"Game/Player/Behavior/IPlayerBehavior.h"

//弱ヒット
class PlayerHit :public PlayerBaseBehavior{

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">プレイヤ</param>
	PlayerHit(Player* player);
	~PlayerHit()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private: ///**パラメータ**//

	//ヒット時の速度減衰
	float multiplySpd_ = 0.5f;

	//この状態の続く時間
	float sec_ = 1.0f;
	float current_ = 0;

private: //**プライベート変数**//
	
	//ぷれいやーポインタ
	Player* player_;
	//デバッグツリー
	GlobalVariableTree tree_ = GlobalVariableTree("ヒット");
};