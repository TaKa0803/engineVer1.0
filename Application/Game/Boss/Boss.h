#pragma once
#include"SingleGameObjects/GameObject.h"
#include"Input/Input.h"
#include"Game/Player/Player.h"
#include"Game/CirccleShadow/CirccleShadow.h"
#include"SphereCollider/SphereCollider.h"
#include"Game/Boss/UI/BossUI.h"
#include"Game/Boss/Behavior/IBossBehavior.h"

#include"Game/Effect/LargeDebris/EffectLargeDebris.h"
#include"Game/Boss/BulletManager/BossBulletManager.h"

//ボスクラス
class Boss : public GameObject {

public: //**パブリック変数：状態管理関係**//

	//各アニメーション
	enum class Animation {
		Idle1,			//待機１
		Idle2,			//待機２
		Idle3,			//待機３

		Walk,			//歩く

		PreCharge,		//タックル構え
		Charging,		//タックル中
		EdCharge,		//タックル後

		PreStump,		//ジャンプ構え
		Fly2Stump,		//ジャンプ中
		Stump,			//落下
		EdStump,		//落下後

		Down,			//ダウン

		Preshot,		//弾発射構え
		Shot,			//発射
		RevShot,		//反転した動きで発射

		PrePunch,		//パンチ構え
		Punch,			//パンチ中

		PreSumersolt,	//サマーソルト前
		SumerSolt,		//サマーソルト中

		BackStep,		//バックステップ

		Grap,			//掴み
		GrapMiss,		//掴みミス

		PreThrow,		//投げ構え
		Throw,			//投げる

		Roll,			//回転

		CountAnimation	//アニメーション数
	};

	/// <summary>
	/// 各アニメーションの文字列
	/// enum class Animationと併用して扱う
	/// </summary>
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
		IDLE,			//待機状態
		DOWN,			//移動
		ATK,			//攻撃
		CountBehavior	//ボスの状態の数
	};

	//攻撃終了感知フラグ
	bool isFinishedATK_ = false;

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	Boss(Player* player);
	~Boss() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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

	/// <summary>
	/// 攻撃ヒット時の処理
	/// </summary>
	void OnCollision();

	/// <summary>
	/// エフェクト有効処理
	/// </summary>
	/// <param name="pos">エフェクト発生座標</param>
	void SpawnStumpEffect(const Vector3& pos) { stumpEffect_->Spawn(pos); }

	/// <summary>
	/// ボスの弾との当たり判定
	/// </summary>
	/// <param name="co">ヒット確認するコライダー</param>
	/// <returns>当たったか否か</returns>
	bool CheckCollisionAmmo(const SphereCollider* co);

public: //**ゲッター**//

	/// <summary>
	/// 体コライダー取得
	/// </summary>
	/// <returns>体コライダー</returns>
	SphereCollider& GetBodyCollider()& { return *bodyCollider_.get(); }

	/// <summary>
	/// 攻撃コライダー取得
	/// </summary>
	/// <returns>攻撃コライダー</returns>
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
	Vector3 GetPlayerWorldTranslate() { return player_->world_.GetWorldTranslate(); };

	/// <summary>
	/// 弾の数を取得
	/// </summary>
	/// <returns>弾の数</returns>
	int GetBulletCount() { return bulletM_->GetBulletCount(); }

public: //**セッター**//

	/// <summary>
	/// 現在の攻撃パターンで攻撃を予約(攻撃番号指定
	/// </summary>
	/// <param name="atkNum">攻撃の番号</param>
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
	void SetAnimation(const std::string& animeName, float sec, float loopSec = 1.0f, bool isLoop = true);

	/// <summary>
	/// アニメーション進行を０-１で管理
	/// </summary>
	/// <param name="active">管理の有効フラグ</param>
	/// <param name="t">０-１のアニメーション進行度</param>
	void SetAnimeTime(bool active, float t = 0) { model_->SetAnimationTime(active, t); }

	/// <summary>
	/// 弾データをセット
	/// </summary>
	/// <param name="data"></param>
	void SetAmmoData(const BossBulletData& data) { bulletM_->SetData(data); }

private://**参照**//

	//入力
	Input* input_ = nullptr;

	//プレイヤー
	Player* player_;

private: //**変数**//

	//ボスーの状態
	Behavior behavior_ = Behavior::IDLE;

	//状態リクエスト
	std::optional<Behavior>behaviorReq_ = std::nullopt;

	//状態群
	std::vector<std::unique_ptr<IBossBehavior>>behaviors_;


	//簡易影
	std::unique_ptr<CirccleShadow>shadow_;

	//コライダー
	std::unique_ptr<SphereCollider> bodyCollider_;
	//攻撃コライダー
	std::unique_ptr<SphereCollider>atkCollider_;

	//UI
	std::unique_ptr<BossUI>ui_;

	//落下攻撃エフェクト
	std::unique_ptr<EffectLargeDebris>stumpEffect_;

	//弾マネージャ
	std::unique_ptr<BossBulletManager> bulletM_;

public: //**パラメータ**//

	//最大体力
	int maxHP_ = 20;

	//現在体力
	int nowHP = 20;

	//死亡フラグ
	bool isDead_ = false;

private: //**デバッグ用**//

	//思考処理フラグ
	bool brein_ = true;

	//当たり判定フラグ
	bool hit_ = true;

	//デバッググループ名
	const std::string groupName_ = "ボス";

	//デバッグ用状態の名前群
	std::string behaviorName_[(int)Behavior::CountBehavior] = {
		"Idle",
		"Move",
		"ATK"
	};
	//現在の状態の名前
	std::string nowBehaviorName_ = "";
};