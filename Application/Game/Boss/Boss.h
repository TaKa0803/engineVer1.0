#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"Game/Player/Player.h"
#include"Game/CirccleShadow/CirccleShadow.h"
#include"SphereCollider/SphereCollider.h"
#include"Game/Boss/UI/BossUI.h"
#include"Game/Boss/Behavior/IBossBehavior.h"

#include"Game/Effect/EffectLargeDebris.h"
#include"Game/Boss/BulletManager/BossBulletManager.h"

//ボスクラス
class Boss : public GameObject {

public://パブリック変数：状態管理関係

	//各アニメーション
	enum class Animation {
		Idle1,
		Idle2,
		Idle3,

		Walk,

		PreCharge,
		Charging,
		EdCharge,

		PreStump,
		Fly2Stump,
		Stump,
		EdStump,

		Down,

		Preshot,
		Shot,
		RevShot,

		PrePunch,
		Punch,
	
		PreSumersolt,
		SumerSolt,

		BackStep,

		Grap,
		GrapMiss,

		PreThrow,
		Throw,

		Roll,

		CountAnimation
	};

	//アニメーション名
	std::string animeName_[(int)Animation::CountAnimation] = {

		"Idle",
		"Idle2",
		"Idle3",
		"walk",

		"preCharge",
		"Charge",
		"nextCharge",

		"preStump",
		"flyStump",
		"Stump",
		"nextStump",

		"DownBody",

		"preShot",
		"ShotSingle",
		"Idle",

		"PrePunch",
		"Punch",

		"PreSumerSolt",
		"SumerSolt",

		"BackStep",

		"Grap",
		"GrapMiss",

		"Prethrow",
		"throw",

		"Roll"
	};

	//ボスの状態
	enum class Behavior {
		IDLE,	//待機状態
		DOWN,	//移動
		ATK,    //攻撃
		CountBehavior
	};

	//攻撃終了感知フラグ
	bool isFinishedATK_ = false;

public://パブリック関数

	//コンストラクタ
	Boss(Player* player);
	~Boss()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	//攻撃ヒット時の処理
	void OnCollision();

	//エフェクト有効処理委取得
	void SpawnStumpEffect(const Vector3& pos) { stumpEffect_->Spawn(pos); }
public://ゲッター

	//体コライダー取得
	SphereCollider& GetBodyCollider()& { return *bodyCollider_.get(); }

	//攻撃用コライダー取得
	SphereCollider& GetATKCollider()& { return *atkCollider_.get(); }

	/// <summary>
	/// プレイヤー方向への向きベクトルを取得
	/// </summary>
	/// <returns>プレイヤー方向の向きベクトル</returns>
	Vector3 GetBoss2PlayerDirection();

	/// <summary>
	/// プレイヤーのワールド座標取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetPlayerWorldTranslate() { return player_->GetWorld().GetWorldTranslate(); };

public://セッター

	//現在の攻撃パターンで攻撃を予約(攻撃番号指定
	void SetNextATK(int atkNum);

	/// <summary>
	/// 状態リクエスト送信
	/// </summary>
	/// <param name="behavior">リクエストしたい状態</param>
	void SetBehavior(Behavior behavior) { behaviorReq_ = behavior; }

	/// <summary>
	/// プレイヤー方向を向く
	/// </summary>
	void SetDirection2Player();

	/// <summary>
	/// アニメーションのセット
	/// </summary>
	/// <param name="animeNum">アニメーション名</param>
	/// <param name="sec">変わりきるまでの秒数</param>
	/// <param name="loopSec">ループ時間</param>
	/// <param name="isLoop">ループフラグ</param>
	void SetAnimation(const std::string& animeName, float sec, float loopSec=1.0f, bool isLoop = true);

	///アニメーション進行をこちらで管理する処理
	void SetAnimeTime(bool active, float t = 0) { model_->SetAnimationTime(active, t); }
private://状態管理


private://**参照

	//入力
	Input* input_ = nullptr;

	//プレイヤー
	Player*player_;

	//弾マネージャ
	BossBulletManager* bulletM_;
private://**変数

	//ボスーの状態
	Behavior behavior_ = Behavior::IDLE;
	//状態リクエスト
	std::optional<Behavior>behaviorReq_ = std::nullopt;
	//簡易影
	std::unique_ptr<CirccleShadow>shadow_;
	//コライダー
	std::unique_ptr<SphereCollider> bodyCollider_;
	//攻撃コライダー
	std::unique_ptr<SphereCollider>atkCollider_;

	//UI
	std::unique_ptr<BossUI>ui_;
	
	//状態群
	std::vector<std::unique_ptr<IBossBehavior>>behaviors_;

	std::unique_ptr<EffectLargeDebris>stumpEffect_;
public://**パラメータ

	int maxHP_ = 20;

	int maxHP = 20;

	bool isDead_ = false;
private://ImGui用

	//思考処理
	bool brein_ = true;

	bool hit_ = true;

	//デバッググループ名
	const std::string groupName_ = "ボス";

	//
	std::string behaviorName_[(int)Behavior::CountBehavior] = {
		"Idle",
		"Move",
		"ATK"
	};
	//現在の状態の名前
	std::string nowBehaviorName_="";
};