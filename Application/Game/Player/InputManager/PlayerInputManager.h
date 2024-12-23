#pragma once
#include"Input/Input.h"

//プレイヤーの入力を管理する処理
class PlayerInputManager {

public://**パブリック変数**//

	//入力タイプ
	enum InputType
	{
		MOVE,			//移動
		DASH,			//ダッシュ
		ROLL,			//回転
		ATK,			//攻撃
		CountInputType	//入力タイプの数
	};

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerInputManager();
	~PlayerInputManager()=default;

	/// <summary>
	/// 入力チェック
	/// </summary>
	/// <param name="type">タイプ</param>
	/// <returns></returns>
	bool CheckInput(InputType type);

	/// <summary>
	/// 移動方向入力を取得して正規化して返却
	/// </summary>
	/// <returns>移動方向の正規化ベクトル</returns>
	Vector3 GetMoveVelo();

private://**プライベート関数**//

	//入力受け取り処理
	Input* input_ = nullptr;

};

//プレイヤーインプットマネージャの省略名
using PInputM = PlayerInputManager;