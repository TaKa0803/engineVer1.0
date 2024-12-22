#pragma once
#include"GlobalVariable/Tree/GlobalVariableTree.h"
class Boss;

//ボスの状態の基底クラス
class IBossBehavior {

public:	//**パブリック関数**//

	/// <summary>
	/// 解放処理
	/// </summary>
	virtual ~IBossBehavior() = default;

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
	GlobalVariableTree& GetTree() { return tree_; };

public:	//**デバッグ用**//

	Boss* boss_=nullptr;

	//デバッグツリー
	GlobalVariableTree tree_ = GlobalVariableTree("");
};