#pragma once
#include"Game/Boss/Behavior/IBossBehavior.h"


//死亡モーション
class BossDown :public IBossBehavior{
public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="boss">ボスポインタ</param>
	BossDown(Boss*boss);
	~BossDown() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

private: //**プライベート変数**//

	//ボスポインタ
	Boss* boss_;

	//アニメーション時間
	float animeSec_ = 1.0f;

	//次に行くまでの時間
	float nextSec_ = 1.0f;

	//カウント
	float currentNext_ = 0;

};