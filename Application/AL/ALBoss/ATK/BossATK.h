#pragma once
#include"AL/ALBoss/ATK/Normal/BossNormal.h"
#include<iostream>

class ALBoss;

//ボスの棒立ちor試案処理
class BossATK {
public:

	BossATK(ALBoss* boss);
	~BossATK() = default;

	//シーンでの初期化
	void SceneInit();

	void Initialize();

	void Update();

private:

	ALBoss* boss_;

private://**ボス攻撃タイプによる変化**//
	enum ModeTypes {
		Normal,
		Katana,
		Bow,
		_countModeType
	};

	ModeTypes modeType = Normal;

	static void(BossATK::* TypeInit[]) ();
	static void(BossATK::* TypeUpdate[]) ();

	void InitNormal();
	void UpdateNormal();

	//通常状態の攻撃処理
	std::unique_ptr<BossNormal>normal_;


private://**変数**//

};