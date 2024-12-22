#pragma once
#include"InstancingGameObject/InstancingGameObject.h"
#include"SphereCollider/SphereCollider.h"
#include"Game/Effect/Normal/EffectNormal.h"
#include<vector>

//弾のデータ
struct BossBulletData {
	EulerWorldTransform world;//座標
	Vector3 velo;//速度
};

//ボスの弾の管理マネージャ
class BossBulletManager :private InstancingGameObject{

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BossBulletManager();
	~BossBulletManager()=default;


	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="anime">モデルのアニメ番号</param>
	void Draw()override;

	/// <summary>
	/// 弾データ登録
	/// </summary>
	/// <param name="data">弾のデータ</param>
	void SetData(const BossBulletData&data);

	/// <summary>
	/// 当たり判定チェック
	/// </summary>
	/// <param name="co">弾群と判定を取るコライダー</param>
	/// <returns></returns>
	bool CheckCollision(const SphereCollider* co);

public: //**ゲッター**//

	/// <summary>
	/// 弾の数取得
	/// </summary>
	/// <returns></returns>
	int GetBulletCount() { return (int)datas_.size(); }

	//ツリーの取得
	GlobalVariableTree& GetTree() { return tree_; }

private: //**プライベート変数**//

	//弾データ群
	struct Datas {
		//弾のデータ
		BossBulletData data;

		//コライダーデータ
		std::unique_ptr<SphereCollider> collider;

		//死ぬまでのカウント
		float deadCount = 0;

		//ヒットフラグ
		bool ishit = false;
	};

	//弾データ管理変数
	std::vector<std::unique_ptr<Datas>>datas_;

	//当たった時のエフェクト
	std::unique_ptr<EffectNormal>hiteffect_;

private: //**パラメータ**//

	//速度
	float spd_=30.0f;

	//死亡までの時間
	float maxDeadSec_ = 1.0f;

	//弾のサイズ
	float ammoScale_ = 0.5f;

	//コライダーサイズ
	float colliderScale_ = 1.0f;
};