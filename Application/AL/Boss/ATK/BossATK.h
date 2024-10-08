#pragma once
#include"AL/Boss/ATK/Normal/BossNormalATKManager.h"
#include<iostream>

class Boss;

//ボスの攻撃管理クラス
class BossATKManager {
public:

	BossATKManager(Boss* boss);
	~BossATKManager() = default;

	//シーンでの初期化
	void SceneInit();

	void Initialize();

	void Update();

private:

	Boss* boss_;

private://**ボス攻撃タイプによる変化**//
	enum ModeTypes {
		Normal,
		Katana,
		Bow,
		_countModeType
	};

	ModeTypes modeType = Normal;

	static void(BossATKManager::* TypeInit[]) ();
	static void(BossATKManager::* TypeUpdate[]) ();


	void InitNormal();
	void UpdateNormal();

	//通常状態の攻撃処理
	std::unique_ptr<BossNormalATKManager>normal_;


private://**変数**//

};