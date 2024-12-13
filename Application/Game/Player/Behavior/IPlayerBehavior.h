#pragma once
#include"GvariGroup/GvariGroup.h"

//プレイヤー前方宣言
class Player;

//プレイヤー状態の基底クラス
class PlayerBaseBehavior {

public:
	//純粋仮想デストラクタ
	virtual ~PlayerBaseBehavior() = default;

	//初期化
	virtual void Initialize() = 0;

	//更新
	virtual void Update() = 0;

	/// <summary>
	/// デバッグ用のツリーデータ取得
	/// </summary>
	/// <returns>ツリーデータ</returns>
	GlobalVariableTree& GetTree() { return tree_; }
public:
	//プレイヤーポインタ
	Player* player_;
	//ツリーデータ
	GlobalVariableTree tree_ = GlobalVariableTree("");
};