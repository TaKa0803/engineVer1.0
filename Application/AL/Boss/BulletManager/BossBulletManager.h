#pragma once
#include"InstancingGameObject/InstancingGameObject.h"
#include"SphereCollider/SphereCollider.h"
#include<vector>

struct BossBulletData {
	EulerWorldTransform world;
	Vector3 velo;
};

class BossBulletManager :public InstancingGameObject{

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
private:

	//データ群
	struct Datas {
		//弾のデータ
		BossBulletData data;

		//コライダーデータ
		std::unique_ptr<SphereCollider> collider;

		//ヒットフラグ
		bool ishit = false;
	};

	std::vector<std::unique_ptr<Datas>>datas_;
};