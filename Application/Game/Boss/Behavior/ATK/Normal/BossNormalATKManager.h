#pragma once
#include"Game/Boss/Behavior/ATK/IATK/IATK.h"
#include"GlobalVariable/Tree/GlobalVariableTree.h"
#include<iostream>
#include<vector>

//ボスの前方宣言
class Boss;

//ノーマル状態の攻撃の管理クラス
class BossNormalATKManager {

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
	/// <param name="contract">指定攻撃番号</param>
	void Initialize(std::optional<int>contract);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ツリーの取得
	/// </summary>
	/// <returns>ツリーの参照データ</returns>
	GlobalVariableTree& GetTree() { return tree; };

private: /**プライベート関数**/

	/// <summary>
	/// デバッグ処理まとめ
	/// </summary>
	void Debug();

private: //**参照物**//

	//ボスのポインタ
	Boss* boss_;

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

	//デバッグ用ツリー
	GlobalVariableTree tree = GlobalVariableTree("normal");

	//攻撃の文字列
	std::string atkTypeName_[CountOfATKData]={
		"落下攻撃",
		"弾発射",
		"タックル",
		//"パンチ",
		//"サマーソルト",
		"バックステップ",
	};

	//現在の攻撃の名前
	std::string nowAtkName_ = "";

	//攻撃指定
	bool isDesignate_ = false;

	//指定攻撃
	int designateType_ = 0;
};