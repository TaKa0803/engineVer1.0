#pragma once
#include"AL/Boss/ATK/IATK/IATK.h"
#include"GvariGroup/GvariGroup.h"
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

	GVariTree& GetTree() { return tree; };

private:
	//デバッグ
	void Debug();

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

	//攻撃状態管理
	std::vector<std::unique_ptr<IBossATK>> typeArr_;
public:
	bool isFinished_ = false;

private://デバッグ
	GVariTree tree = GVariTree("normal");

	std::string atkTypeName_[CountOfATKData]={
		"落下攻撃",
		"弾発射",
		"タックル"
	};

	std::string nowAtkName_ = "";

	//攻撃指定
	bool isDesignate_ = false;
	//指定攻撃
	int designateType_ = 0;
};