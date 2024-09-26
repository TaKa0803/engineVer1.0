#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SphereCollider/SphereCollider.h"
#include"AL/items.h"
#include"Effect/EffectMove/EffectMove.h"
#include"AL/CirccleShadow/CirccleShadow.h"

#include"PlayerRoll/PlayerRoll.h"

#include<vector>



//攻撃のデータ構造体
struct ATKData {
	float extraTime = 0;	//予備動作
	float AttackTime = 0;	//攻撃動作
	float RigorTime = 0;	//硬直時間

	int isYATK = false;	//Yボタン攻撃か

	float spd = 0;

	ModelAnimeParts parts;

	std::vector<ATKData>ATKDerivation;	//攻撃の派生
};

class ALPlayer :public GameObject {

public:
	ALPlayer();
	~ALPlayer();

	void Initialize();

	void Update();

	void Draw();

	void DrawParticle();

	void DebugWindow(const char* name);

	void SetCamera(Camera* camera) { camera_ = camera; }

	//押し戻し
	void OnCollisionBack(const Vector3& backV);

	SphereCollider* GetCollider() { return collider_.get(); }

	int GetConboCount() { return ATKConboCount; }

	//プレイヤー方向取得
	Vector3 GetDirection();

	bool IsPlayerATK() {
		if (behavior_ == State::ATK) { return true; }
		return false;
	}

	enum class State {
		Move,		//移動
		Rolling,    //ローリング 
		ATK,		//攻撃
		HITACTION,	//被攻撃時
		kNumStates	//状態の数
	};
private://メンバ関数


	//移動
	void Move();

	void ModelRoop(const Vector3& velo);

#pragma region 状態管理とメンバ関数ポインタテーブル



	//プレイヤーの状態
	State behavior_ = State::Move;


	//状態ごとの初期化テーブル
	static void (ALPlayer::* BehaviorInitialize[])();

	//状態ごとの更新テーブル
	static void (ALPlayer::* BehaviorUpdate[])();


	void InitMove();

	void InitRolling();

	void InitATK();

	void InitHitAction();


	void UpdateMove();

	void UpdateRolling();

	void UpdateATK();

	void UpdateHitAction();

#pragma endregion

	void LoadATKDatas();

	void StaminaUpdate();
public:

	struct StaminaData {
		//スタミナ関係
		float maxStamina = 100;
		float currentStamina = StaminaData::maxStamina;

		//回復開始カウント
		float reChargeSec = 1.0f;
		float currentCharge = 0.0f;

		//回復速度
		float healSec = 100;

		//各アクションのスタミナコスト
		float rollCost = 20;

		float dashCostSec = 10;

		float atkCost = 10;
	};

	struct PlayerData
	{

		Vector3 velo_ = { 0,0,0 };
		Vector3 acce_ = { 0,0,0 };

		//移動速度
		float spd_ = 0.5f;

		//ダッシュの速度倍率
		float dashMultiply = 2.0f;

		//落下速度
		float fallSpd_ = 0.1f;

		//加算式落下加速度
		float addFallSpd_ = 0;

		StaminaData stamina{};
	};

	PlayerData data_;

	//状態リクエスト
	std::optional<State>behaviorReq_ = std::nullopt;

private:
	Input* input_ = nullptr;

	const Camera* camera_ = nullptr;

	std::unique_ptr<SphereCollider> collider_;

	std::unique_ptr<EffectMove>peM_;

	std::unique_ptr<PlayerRoll>rolling_;

	std::unique_ptr<CirccleShadow>shadow_;

#pragma region モデルに関する

	//タグ軍

	int textureData = 0;
#pragma endregion





	enum MoveState {
		StopS,
		MoveS,
		NoneS
	};

	MoveState moveState_ = NoneS;

#pragma region 攻撃に関する変数

	//std::unique_ptr<EffectImpact>impactE_;

	//攻撃に関するデータ
	std::string atkDataPass_ = "resources/jsonfile/PlayerATKData.json";

	//グループ名
	std::string groupName_ = "BButtonATK";

	static const int itemNum = 2;

	//アイテムの名前
	std::string keyNames_[itemNum] = {
		"data",
		"ATKDerivation"
	};


	//ボタンを押したときからの攻撃一覧
	ATKData startATKData_;





	//最大コンボ数
	const int maxATKConBo = 3;

	int ATKConboCount = 1;

	enum class ATKState {
		Extra,//準備
		ATK,	//攻撃実行
		Rigor//硬直
	};
	//攻撃の状態enum
	ATKState atkState_;

	//攻撃時に使うデータまとめ
	struct ATKUpdateData {
		//次の攻撃をするか
		bool nextATK = false;
		bool isPushY = false;
		int count = 0;
	};

	//攻撃の更新データ
	ATKUpdateData updateATKData_;

	//実行する攻撃動作
	ATKData ATKData_;

	bool ATKAnimationSetup_ = false;

	enum NowATK {
		kATK1,
		kATK2,
		kATK3,
		_countATK
	};

	NowATK nowATKState_ = kATK1;
#pragma endregion


	//音のデータポインタ
	int punchSound_;

	int kickSound_;

	int drilSound_;



};