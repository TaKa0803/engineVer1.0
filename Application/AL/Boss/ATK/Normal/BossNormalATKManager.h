#pragma once
#include"AL/Boss/ATK/IATK/IATK.h"

#include<iostream>
#include<vector>

class Boss;

//ノーマル状態の攻撃の管理クラス
class BossNormalATKManager {

public:
	BossNormalATKManager(Boss*boss);
	~BossNormalATKManager() = default;

	void Initialize();

	void Update();

private://参照してきたもの

	Boss* boss_;

private://**ノーマルの攻撃手段

	//攻撃タイプ
	enum ATKType {
		Stump,			//潰す攻撃
		ShotBullet,		//弾発射
		Charge,			//突撃
		CountOfATKData
	};

	//攻撃タイプ
	ATKType type_=ATKType::Stump;

	//仮追加
	std::vector<std::unique_ptr<IBossATK>> typeArr_;
public:
	bool isFinished_ = false;
};