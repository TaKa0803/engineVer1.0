#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"
#include<iostream>

//前方宣言
class Boss;

//ボスの攻撃
class IBossATKTypeManager {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ、デストラクタはデフォルト
	/// </summary>
	IBossATKTypeManager() = default;
	virtual ~IBossATKTypeManager() = default;
	
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// 予定攻撃番号セット
	/// </summary>
	/// <param name="num"></param>
	void SetATKNum(int num) { contract_ = num; }

	/// <summary>
	/// ツリーデータ取得
	/// </summary>
	/// <returns>データ</returns>
	GvariTree& GetTree() { return tree; }

public://**参照物**//

	//ボスのポインタ
	Boss* boss_ = nullptr;

public://**パブリック変数**//

	//攻撃状態群
	std::vector<std::unique_ptr<BossBaseATK>> typeArr_;

	//予定された攻撃
	std::optional<int>contract_ = std::nullopt;

public://**デバッグ用変数*//

	//デバッグ用ツリー
	GlobalVariableTree tree = GlobalVariableTree("");

	//現在の攻撃の名前
	std::string nowAtkName_ = "";

	//攻撃指定フラグ
	bool isDesignate_ = false;

	//指定した攻撃
	int designateType_ = 0;
};