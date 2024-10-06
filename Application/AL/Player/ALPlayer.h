#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SphereCollider/SphereCollider.h"
#include"AL/items.h"
#include"Effect/EffectMove/EffectMove.h"
#include"AL/CirccleShadow/CirccleShadow.h"
#include"AL/Player/Behavior/PlayerATKManager/PlayerATKManager.h"


#include"PlayerRoll/PlayerRoll.h"

#include<vector>

#pragma region アニメーション関係
enum AnimationData {
	Dash,
	Idle,
	PrePunch1,
	PrePunch2,
	PrePunch3,
	Punch1,
	Punch2,
	Punch3,
	Roll,
	RollEnd,
	Run,
	_countAnime
};
#pragma endregion

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

	void OnCollision();

	//押し戻し
	void OnCollisionBack(const Vector3& backV);

	SphereCollider* GetCollider() { return collider_.get(); }

	SphereCollider* GetATKCollider() { return atkCollider_.get(); };

	/// <summary>
	/// 入力方向に向く
	/// </summary>
	/// <param name="isZero">入力の有無受け取り</param>
	/// <returns></returns>
	Vector3 SetInputDirection(bool& isZero);
	
	//int GetConboCount() { return ATKConboCount; }

	bool IsPlayerATK() {
		if (behavior_ == State::ATK) { return true; }
		return false;
	}

	//攻撃入力の取得処理まとめ
	bool GetATKInput();
	//回転入力の処理まとめ
	bool GetRollInput();

	enum class State {
		Move,		//移動
		Rolling,    //ローリング 
		ATK,		//攻撃
		HITACTION,	//被攻撃時
		kNumStates	//状態の数
	};

	//アニメーションのセット
	void SetAnimation(int animeNum, float sec, float loopSec, bool isLoop = true);
	
	///アニメーション進行をこちらで管理する処理
	void SetAnimeTime(bool active, float t = 0) { model_->SetAnimationTime(active, t); }
private://メンバ関数


	//移動
	void Move();

	void ModelRoop(bool ismove, bool isDash);



	void StaminaUpdate();
private://状態管理関数

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

		//体力
		int HP_ = 10;

		//移動速度
		float spd_ = 13.0f;

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
	std::unique_ptr<SphereCollider> atkCollider_;


	std::unique_ptr<EffectMove>peM_;

	std::unique_ptr<PlayerRoll>rolling_;

	std::unique_ptr<PlayerATKManager>atkM_;

	std::unique_ptr<CirccleShadow>shadow_;

#pragma region モデルに関する

	//タグ軍

	int textureData = 0;
#pragma endregion





};