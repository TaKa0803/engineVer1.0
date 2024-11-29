#pragma once
#include"Game/Boss/Behavior/ATK/Normal/BossNormalATKManager.h"
#include"Game/Boss/Behavior/IBossBehavior.h"
#include<iostream>



//ボスの攻撃管理クラス
class BossATKTypeManager :public IBossBehavior{
public:

	BossATKTypeManager(Boss* boss);
	~BossATKTypeManager() = default;

	//シーンでの初期化
	void SceneInit();

	void Initialize()override;

	void Update()override;

	/// <summary>
	/// 現在の攻撃パターンから攻撃を予約
	/// </summary>
	/// <param name="atknum"></param>
	void GetATKContract(int atknum) { contract_ = atknum; };
private:
	//ボスのポインタ
	Boss* boss_;

private://**ボス攻撃タイプによる変化**//
	enum ModeTypes {
		Normal,
		Katana,
		Bow,
		_countModeType
	};

	ModeTypes modeType = Normal;

	static void(BossATKTypeManager::* TypeInit[]) (std::optional<int>contract);
	static void(BossATKTypeManager::* TypeUpdate[]) ();


	void InitNormal(std::optional<int>contract);
	void UpdateNormal();

	//通常状態の攻撃処理
	std::unique_ptr<BossNormalATKManager>normal_;


private://**変数**//

	std::optional<int> contract_=std::nullopt ;

private://デバッグ
	std::string typeName_[_countModeType]{
		"Normal",
		"Katana",
		"Bow"
	};

	std::string nowTypeName_ = "";
};