#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SphereCollider/SphereCollider.h"
#include"Game/items.h"
#include"Effect/EffectMove/EffectMove.h"
#include"Game/CirccleShadow/CirccleShadow.h"
#include"Game/Player/Behavior/PlayerATKManager/PlayerATKManager.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"

#include"PlayerRoll/PlayerRoll.h"

#include<vector>
//#include<string>

class Boss;

class Player :public GameObject {

public://パブリック変数

#pragma region アニメーション関係
	enum  AnimationData {

		Idle,	//待機
		Moving,	//移動
		Dash,	//走る

		Roll,	//回転
		RollEnd,//回転終わり

		PrePunch1,//パンチ１構え
		PrePunch2,//パンチ２構え
		PrePunch3,//パンチ３構え
		Punch1,//パンチ1
		Punch2,//パンチ2
		Punch3,//パンチ3

		Dawn,//ダウン
		DawnBack,//ダウン復帰
		CountAnime//アニメーションの数
	};

	//アニメーション名まとめ
	std::string animeName_[CountAnime] = {
		"Idle",
		"walk",
		"Dash",

		"Rolling",
		"RollingEnd",

		"PrePunch",
		"PrePunch2",
		"PrePunch3",
		"Punch",
		"Punch2",
		"Punch3",

		"Down",
		"DownBack",



	};

#pragma endregion

	//状態
	enum class Behavior {
		IDLE,		//移動
		Rolling,    //ローリング 
		ATK,		//攻撃
		HITACTION,	//被攻撃時
		kNumStates	//状態の数
	};

#pragma region パラメータ
	struct StaminaData {
		//スタミナ関係
		float maxStamina = 100;
		float currentStamina = StaminaData::maxStamina;

		//回復開始カウント
		float rechargeSec = 1.0f;
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

		//ヒットカウント
		float currentHitCount_ = 0.0f;

		//無敵時間
		float noHitTime_ = 1.0f;

		//スタミナデータ
		StaminaData stamina{};
	};

	PlayerData data_;
#pragma endregion

public:

	//コンストラクタ
	Player();
	~Player() = default;

	//初期化
	void Initialize();

	//ボスポインタ取得
	void GetBoss(const Boss* boss);

	//カメラ取得
	void SetCamera(Camera* camera);

	//更新
	void Update();

	//描画
	void Draw();

	//パーティクル描画
	void DrawParticle();

	//ヒット時の処理
	void OnCollision();

	/// <summary>
	/// 押し戻し処理
	/// </summary>
	/// <param name="backV">押し戻し量</param>
	void OnCollisionBack(const Vector3& backV);


	//移動処理
	void Move(bool isDash = true, float spdMulti = 1.0f);

	//入力による状態変更処理
	void ChangeBehavior();

	//ローリングによるスタミナ減少処理
	void DecreaseStamina4Roll();

	//攻撃時の初期化処理
	void InitATK();

public://ゲッター

	//コライダー取得
	SphereCollider* GetCollider() { return collider_.get(); }

	//攻撃コライダー取得
	SphereCollider* GetATKCollider() { return atkCollider_.get(); };

	/// <summary>
	/// 入力方向に向く
	/// </summary>
	/// <param name="isZero">入力の有無受け取り</param>
	/// <returns></returns>
	Vector3 SetInputDirection(bool& isZero);

	//プレイヤーからボスへの向きベクトル
	const Vector3 GetP2BossVelo();

	//攻撃入力の取得処理まとめ
	bool GetATKInput();
	//回転入力の処理まとめ
	bool GetRollInput();
	//ダッシュ入力の処理まとめ
	bool GetDashInput();


public://セッター

	/// <summary>
	/// アニメーチョン変更
	/// </summary>
	/// <param name="animeName">アニメーション名</param>
	/// <param name="sec">変化までの秒数</param>
	/// <param name="loopSec">ループまでの秒数</param>
	/// <param name="isLoop">ループのフラグ</param>
	void SetAnimation(const std::string& animeName, float sec, float loopSec, bool isLoop = true);

	///アニメーション進行をこちらで管理する処理
	void SetAnimeTime(bool active, float t = 0) { model_->SetAnimationTime(active, t); }

	//状態リクエスト
	void SetBehaviorReq(Behavior behavior) { behaviorReq_ = behavior; }

private://メンバ関数

	//アニメーションによるモデルの変更
	void ModelRoop(bool ismove, bool isDash);

	//スタミナの更新処理
	void StaminaUpdate();

private://状態管理

	//プレイヤーの状態
	Behavior behavior_ = Behavior::IDLE;
	
	//状態リクエスト
	std::optional<Behavior>behaviorReq_ = std::nullopt;
	
	//状態処理
	std::vector<std::unique_ptr<IPlayerBehavior>>behaviors_;

public://パラメータ



	

private://ポインタ参照
	Input* input_ = nullptr;

	const Camera* camera_ = nullptr;

	const Boss* boss_;

private://変数

	std::unique_ptr<SphereCollider> collider_;
	std::unique_ptr<SphereCollider> atkCollider_;

	//移動エフェクト
	std::unique_ptr<EffectMove>moveE_;
	//丸い影
	std::unique_ptr<CirccleShadow>shadow_;


	//あたるかの判定
	bool isHit_ = true;

private://デバッグ用
	
#ifdef _DEBUG
	bool debugIsHit_ = true;
#endif // _DEBUG


};