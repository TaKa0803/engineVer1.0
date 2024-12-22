#pragma once
#include"GlobalVariable/Tree/GlobalVariableTree.h"

//前方宣言
class Player;

//プレイヤー攻撃基底クラス
class BasePlayerATK {

public://**パブリック関数**//

	/// <summary>
	/// 仮想デストラクタはdefault
	/// </summary>
	virtual ~BasePlayerATK() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize()=0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// ツリー取得
	/// </summary>
	/// <returns></returns>
	GlobalVariableTree& GetTree() { return tree_; }

public://**プライベート変数**//

	//プレイヤーポインタ
	Player* player_;

public://**デバッグ用値

	//ツリー
	GlobalVariableTree tree_ = GlobalVariableTree("");

};