#pragma once
#include"Game/Boss/Behavior/ATK/IATKManager/IBossATKManager.h"
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"
#include"GlobalVariable/Tree/GlobalVariableTree.h"
#include<iostream>
#include<vector>

//ノーマル状態の攻撃の管理クラス
class BossNormalATKManager :public IBossATKManager {

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="boss">ボスのポインタ</param>
	BossNormalATKManager(Boss*boss);
	~BossNormalATKManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

private: /**プライベート関数**/

	/// <summary>
	/// デバッグ処理まとめ
	/// </summary>
	void Debug();

private://**ノーマルの攻撃手段**//

	//攻撃タイプ
	enum ATKType {
		Stump,			//潰す攻撃
		ShotBullet,		//弾発射
		Charge,			//突撃
		//Punch,
		//SumerSolt,
		BackStep,		//バックステップ
		CountOfATKData	//攻撃タイプの数
	}type_ = ATKType::Stump;

	//攻撃状態群
	std::vector<std::unique_ptr<BossBaseATK>> typeArr_;

private: //**デバッグ変数**//

	//攻撃の文字列
	std::string atkTypeName_[CountOfATKData]={
		"落下攻撃",
		"弾発射",
		"タックル",
		//"パンチ",
		//"サマーソルト",
		"バックステップ",
	};
};