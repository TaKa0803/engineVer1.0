#pragma once
#include"Game/Boss/ATK/Normal/BossNormalATKManager.h"

#include<iostream>

class Boss;

//ボスの攻撃管理クラス
class BossATKTypeManager {
public:

	BossATKTypeManager(Boss* boss);
	~BossATKTypeManager() = default;

	//シーンでの初期化
	void SceneInit();

	void Initialize();

	void Update();

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

	static void(BossATKTypeManager::* TypeInit[]) ();
	static void(BossATKTypeManager::* TypeUpdate[]) ();


	void InitNormal();
	void UpdateNormal();

	//通常状態の攻撃処理
	std::unique_ptr<BossNormalATKManager>normal_;


private://**変数**//

private://デバッグ
	std::string typeName_[_countModeType]{
		"Normal",
		"Katana",
		"Bow"
	};

	std::string nowTypeName_ = "";
};