#pragma once
#include"Game/Boss/Behavior/IBossBehavior.h"
#include"Game/Boss/Behavior/ATK/IATKManager/IBossATKTypeManager.h"

#include<iostream>



//ボスの攻撃管理クラス
class BossATKTypeManager :public IBossBehavior{
public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="boss">ボスのポインタ</param>
	BossATKTypeManager(Boss* boss);
	~BossATKTypeManager() = default;

	/// <summary>
	///シーンでの初期化 
	/// </summary>
	void SceneInit();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 現在の攻撃パターンの攻撃を予約
	/// </summary>
	/// <param name="atknum"></param>
	void GetATKContract(int atknum) { plannedATK_ = atknum; };

private: //**ボス攻撃タイプによる変化**//
	
	//ボスの攻撃タイプ
	enum ModeTypes {
		Normal,			//通常
		Katana,			//カタナ持ち
		Bow,			//弓
		CountModeType	//攻撃タイプ数
	};

	//現在のタイプ
	ModeTypes modeType = Normal;

	//ボスの攻撃タイプ群
	std::vector<std::unique_ptr<IBossATKTypeManager>>behaviors_;

private: //**変数**//

	//予定されている攻撃番号
	std::optional<int> plannedATK_=std::nullopt ;

private: //**デバッグ**//

	//攻撃タイプの名前群
	std::string typeName_[CountModeType]{
		"Normal",	//通常
		"Katana",	//カタナ
		"Bow"		//弓
	};

	//現在の攻撃タイプの名前
	std::string nowTypeName_ = "";
};