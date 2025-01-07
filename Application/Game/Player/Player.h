#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SphereCollider/SphereCollider.h"
#include"Game/Effect/Move/EffectMove.h"
#include"Game/CirccleShadow/CirccleShadow.h"
#include"Game/Player/Behavior/PlayerATKManager/PlayerATKManager.h"
#include"Game/Player/Behavior/IPlayerBehavior.h"
#include"Game/Effect/Normal/EffectNormal.h"

#include"Game/Player/Stamina/PlayerStamina.h"
#include"Game/Player/InputManager/PlayerInputManager.h"
#include"Game/Player/UI/PlayerUI.h"
#include"Game/Player/AnimationManager/PlayerAnimationManager.h"

#include<vector>


class Player :public GameObject {

public://**パブリック変数**//

#pragma region アニメーション関係

#pragma endregion

	//状態
	enum class Behavior {
		IDLE,		//移動
		Rolling,    //ローリング 
		ATK,		//攻撃
		HITACTION,	//被攻撃時
		KNOCKBACK,
		kNumStates	//状態の数
	};

#pragma region パラメータ


	struct PlayerData
	{

		Vector3 velo_ = { 0,0,0 };
		Vector3 acce_ = { 0,0,0 };

		//体力
		float nowHP = 10;

		//かうんとHP
		float currentHP = 0;

		//死亡判定
		bool isDead = false;

		//移動速度
		float spd_ = 13.0f;

		//ダッシュの速度倍率
		float dashMultiply = 2.0f;

		//ヒットカウント
		float currentHitCount_ = 0.0f;

		//無敵時間
		float noHitTime_ = 1.0f;

		//攻撃終了フラグ
		bool isEndATK_ = false;
	}data_;
#pragma endregion

	//プレイヤー入力管理クラス
	std::unique_ptr<PlayerInputManager>input_;

	//アニメーション管理クラス
	std::unique_ptr<PAnimeM>animeManager_;

public://**パブリック関数**//

	//コンストラクタ
	Player();
	~Player() = default;

	//初期化
	void Initialize();

	//ボスポインタ取得
	void GetBossWorld(const EulerWorldTransform* bossWorld);

	//カメラ取得
	void SetCamera(Camera* camera);

	//更新
	void Update();

	//描画
	void Draw();

	//パーティクル描画
	void DrawParticle();

	void DrawUI();

	//ヒット時の処理
	void OnCollision();

	/// <summary>
	/// 攻撃ヒット時処理
	/// </summary>
	void OnCollisionATKHit();

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
	SphereCollider* GetCollider() { return bodyCollider_.get(); }

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

	/// <summary>
	/// 攻撃時のスタミナ取得
	/// </summary>
	/// <returns></returns>
	bool GetStaminaOfATK();

public://セッター

	//状態リクエスト
	void SetBehaviorReq(Behavior behavior) { behaviorReq_ = behavior; }

private://メンバ関数

	//全状態共通初期化
	void GlobalInitialize();

	//全体共通更新
	void GlobalUpdate();

	//アニメーションによるモデルの変更
	void ModelRoop(bool ismove, bool isDash);

private://状態管理

	//プレイヤーの状態
	Behavior behavior_ = Behavior::IDLE;
	
	//状態リクエスト
	std::optional<Behavior>behaviorReq_ = std::nullopt;
	
	//状態処理
	std::vector<std::unique_ptr<PlayerBaseBehavior>>behaviorArr_;
	

private://ポインタ参照

	const Camera* camera_ = nullptr;

	const EulerWorldTransform* bossWorld_=nullptr;

private://変数
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager>hitPariticle;

	std::unique_ptr<SphereCollider> bodyCollider_;
	std::unique_ptr<SphereCollider> atkCollider_;

	//UIデータ
	std::unique_ptr<PlayerUI>UI_;

	//スタミナ管理
	std::unique_ptr<PlayerStamina>stamina_;

	//移動エフェクト
	std::unique_ptr<EffectMove>moveE_;
	//丸い影
	std::unique_ptr<CirccleShadow>shadow_;

	//ヒット時のエフェクト
	std::unique_ptr<EffectNormal>effectHit_;

	//あたるかの判定
	bool isHit_ = true;

private://パラメータ

	//アニメーションブレンド時間
	float animeBlend_ = 0.1f;

	//待機アニメ速度
	float idleAnimeMulti_ = 1.0f;

	//歩きアニメ速度
	float moveAnimeMulti_ = 1.0f;

	//走りアニメ速度
	float runAnimeMulti_ = 1.0f;


private://デバッグ用
	
#ifdef _DEBUG
	bool debugIsHit_ = true;
#endif // _DEBUG


};