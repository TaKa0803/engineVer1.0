#pragma once
#include"Game/Boss/Behavior/ATK/Normal/BossNormalATKManager.h"
#include"Game/Boss/Behavior/IBossBehavior.h"
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

private: //**参照物**//
	//ボスのポインタ
	Boss* boss_;

private: //**ボス攻撃タイプによる変化**//
	
	//ボスの攻撃タイプ
	enum ModeTypes {
		Normal,			//通常
		Katana,			//カタナ持ち
		Bow,			//弓
		_countModeType	//攻撃タイプ数
	};

	//現在のタイプ
	ModeTypes modeType = Normal;

	//初期化関数テーブル
	static void(BossATKTypeManager::* TypeInit[]) (std::optional<int>contract);
	//更新関数テーブル
	static void(BossATKTypeManager::* TypeUpdate[]) ();

	//通常初期化
	void InitNormal(std::optional<int>contract);
	//更新初期化
	void UpdateNormal();

	//通常状態の攻撃処理
	std::unique_ptr<BossNormalATKManager>normal_;

private: //**変数**//

	//予定されている攻撃番号
	std::optional<int> plannedATK_=std::nullopt ;

private: //**デバッグ**//

	//攻撃タイプの名前群
	std::string typeName_[_countModeType]{
		"Normal",	//通常
		"Katana",	//カタナ
		"Bow"		//弓
	};

	//現在の攻撃タイプの名前
	std::string nowTypeName_ = "";
};