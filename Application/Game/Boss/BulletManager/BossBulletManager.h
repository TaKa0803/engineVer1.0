#pragma once
#include"InstancingGameObject/InstancingGameObject.h"
#include"SphereCollider/SphereCollider.h"
#include<vector>

struct BossBulletData {
	EulerWorldTransform world;
	Vector3 velo;
};

class BossBulletManager :private InstancingGameObject{

public:
	static BossBulletManager* GetInstance();
private:
	BossBulletManager() = default;
	~BossBulletManager() = default;
	BossBulletManager(const BossBulletManager& o) = delete;
	const BossBulletManager& operator=(const BossBulletManager& o) = delete;


public:
	/// <summary>
	/// 一度呼び出せばよい
	/// </summary>
	void SetUp();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="anime">モデルのアニメ番号</param>
	void Draw(int anime =0)override;

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

public://ゲッター

	/// <summary>
	/// 弾の数取得
	/// </summary>
	/// <returns></returns>
	int GetBulletCount() { return (int)datas_.size(); }

	//ツリーの取得
	GVariTree& GetTree() { return tree_; }
private:

	//データ群
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

	std::vector<std::unique_ptr<Datas>>datas_;

private://パラメータ
	float spd_=30.0f;

	//死亡までの時間
	float maxDeadSec_ = 1.0f;

	//弾のサイズ
	float ammoScale_ = 0.5f;

	//コライダーサイズ
	float colliderScale_ = 1.0f;
};