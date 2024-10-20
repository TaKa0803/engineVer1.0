#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"AL/Player/ALPlayer.h"
#include"AL/CirccleShadow/CirccleShadow.h"
#include"SphereCollider/SphereCollider.h"

#include"AL/Boss/Idle/BossIdle.h"
#include"AL/Boss/Move/BossMove.h"
#include"AL/Boss/ATK/BossATKTypeManager.h"
#include"AL/Boss/BulletManager/BossBulletManager.h"

//ボスクラス
class Boss : public GameObject {

public://パブリック変数：状態管理関係

	enum class Animation {
		Charging,
		Down,
		Fly2Stump,
		Idle1,
		Idle2,
		Idle3,
		EdCharge,
		EdStump,
		PreCharge,
		Preshot,
		PreStump,
		Shot,
		RevShot,
		Stump,
		Walk,
		CountAnimation
	};

	//ボスの状態
	enum class Behavior {
		IDLE,	//待機状態
		MOVE,	//移動
		ATK,    //攻撃
		_CountBehavior
	};

	//攻撃終了感知フラグ
	bool isFinishedATK_ = false;

public://パブリック関数
	Boss(ALPlayer* player);
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

	//攻撃ヒット時の処理
	void OnCollision();

public://ゲッター

	//体コライダー取得
	SphereCollider& GetBodyCollider()& { return *collider_.get(); }

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
	/// <param name="animeNum">アニメーション番号</param>
	/// <param name="sec">変わりきるまでの秒数</param>
	/// <param name="loopSec">ループ時間</param>
	/// <param name="isLoop">ループフラグ</param>
	void SetAnimation(int animeNum, float sec, float loopSec, bool isLoop = true);

	///アニメーション進行をこちらで管理する処理
	void SetAnimeTime(bool active, float t = 0) { model_->SetAnimationTime(active, t); }
private://状態管理

	//状態ごとの初期化テーブル
	static void (Boss::* BehaviorInitialize[])();

	//状態ごとの更新テーブル
	static void (Boss::* BehaviorUpdate[])();

#pragma region 各状態
	void InitIdle();
	void InitMove();
	void InitATK();

	void UpdateIdle();
	void UpdateMove();
	void UpdateATK();
#pragma endregion
private://**参照

	//入力
	Input* input_ = nullptr;

	//プレイヤー
	ALPlayer*player_;

	//弾マネージャ
	BossBulletManager* bulletM_;
private://**変数

	//ボスーの状態
	Behavior behavior_ = Behavior::IDLE;
	//状態リクエスト
	std::optional<Behavior>behaviorReq_ = std::nullopt;

	std::unique_ptr<CirccleShadow>shadow_;

	std::unique_ptr<SphereCollider> collider_;

	//各処理
	std::unique_ptr<BossIdle>idle_;
	std::unique_ptr<BossMove>move_;
	std::unique_ptr<BossATKTypeManager>atk_;
	

	std::unique_ptr<SphereCollider>atkCollider_;

public://**パラメータ

	int HP_ = 20;

	bool isDead_ = false;
private://ImGui用

	//
	std::string behaviorName_[(int)Behavior::_CountBehavior] = {
		"Idle",
		"Move",
		"ATK"
	};
};