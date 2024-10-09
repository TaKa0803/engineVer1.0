#pragma once
#include<optional>

class Boss;

class IATK {

protected:
	//終了フラグ
	static bool isEnd_;
	//プレイヤーポインタ
	static Boss* boss_;
public:

	//IATKの内容初期化(必要
	void InitIATK();

	//状態の初期化と更新(必須
	void BehaviorUpdating();

	static void SetBossPointer(Boss*boss);
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init();
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();


public://状態のデータ
	//各状態の初期化
	virtual void InitAIMing() = 0;
	virtual void InitWarning() = 0;
	virtual void InitATK() = 0;
	virtual void InitStiffness() = 0;
	virtual void InitBack() = 0;

	//各状態の更新
	virtual void UpdateAIMing() = 0;
	virtual void UpdateWarning() = 0;
	virtual void UpdateATK() = 0;
	virtual void UpdateStiffness() = 0;
	virtual void UpdateBack() = 0;
public://**変数
	enum Behavior {
		AIMing,		//プレイヤーを狙う処理
		Warning,	//警告処理
		ATK,		//実際の攻撃処理
		Stiffness,	//攻撃後の硬直
		Back,		//別攻撃または終わりの処理
		CountOfATKBehavior
	};

	Behavior behavior_ = AIMing;
	std::optional<Behavior>behaviReq_ = std::nullopt;

	//各状態の待機時間（利用しなくても可
	struct BehaviorCountSec
	{
		float aimingSec;		//AIM状態の時間
		float warningSec;		//Warning状態の時間
		float ATKSec;			//ATK状態の時間
		float stiffnessSec;		//Stiffness状態の時間
		float backSec;			//終了状態の時間
	}parameters_;

	//経過カウント
	float currentCount_ = 0;


private:
	static void (IATK::* behaviorInit[])();
	static void (IATK::* behaviorUpdate[])();

};