#pragma once
#include"GlobalVariable/Tree/GlobalVariableTree.h"
#include<optional>

//ボスの前方宣言
class Boss;

//ボス攻撃の基底くらす
class BossBaseATK {

protected: //**共通変数**//

	//ボスポインタ
	static Boss* boss_;

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BossBaseATK();
	virtual ~BossBaseATK() = default;

	/// <summary>
	/// ボスポインタ取得（一回のみ
	/// </summary>
	/// <param name="boss"></param>
	static void SetBossPointer(Boss* boss);

	/// <summary>
	/// IATKの内容初期化(必須
	/// </summary>
	void InitIATK();

	/// <summary>
	/// 状態の初期化と更新(必須
	/// </summary>
	void UpdateBehavior();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// ツリー取得
	/// </summary>
	/// <returns>ツリー</returns>
	GlobalVariableTree& GetTree() { return treeData_; }

	/// <summary>
	/// ツリーに自動でパラメータ追加
	/// </summary>
	void SetParam2Tree(const std::string&treename);
public://状態のデータ

	//各状態の初期化

	virtual void InitAIMing() = 0;		//ターゲット処理
	virtual void InitWarning() = 0;		//攻撃前処理
	virtual void InitATK() = 0;			//攻撃処理
	virtual void InitStiffness() = 0;	//攻撃後の硬直処理
	virtual void InitBack() = 0;		//終了処理

	//各状態の更新
	virtual void UpdateAIMing() = 0;
	virtual void UpdateWarning() = 0;
	virtual void UpdateATK() = 0;
	virtual void UpdateStiffness() = 0;
	virtual void UpdateBack() = 0;
public: //**パブリック変数**//

	enum Behavior {
		AIMing,		//プレイヤーを狙う処理
		Warning,	//警告処理
		ATK,		//実際の攻撃処理
		Stiffness,	//攻撃後の硬直
		Back,		//別攻撃または終わりの処理
		CountOfATKBehavior
	};

	//状態リクエスト
	std::optional<Behavior>behaviReq_ = std::nullopt;

	//各状態の待機時間（利用しなくても可
	struct BehaviorCountSec
	{
		float aimingSec=1.0f;		//AIM状態の時間
		float warningSec = 1.0f;		//Warning状態の時間
		float ATKSec = 1.0f;			//ATK状態の時間
		float stiffnessSec = 1.0f;		//Stiffness状態の時間
		float backSec = 1.0f;			//終了状態の時間
	}parameters_{};

	//経過カウント
	float currentCount_ = 0;

private: //**状態関係**//
	//現在の状態
	Behavior behavior_ = AIMing;

	//関数テーブル
	static void (BossBaseATK::* behaviorInit[])();
	static void (BossBaseATK::* behaviorUpdate[])();

public: //**デバッグ関連**//

	//ツリー
	GlobalVariableTree treeData_ = GlobalVariableTree("");

	//各状態の名前
	std::string behaviorName_[CountOfATKBehavior] = {
		"狙う",
		"攻撃前",
		"実攻撃",
		"攻撃後硬直",
		"硬直後復帰"
	};

	//現在の状態の名前
	std::string nowBehavior_ = "";

};