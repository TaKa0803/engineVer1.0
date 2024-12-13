#pragma once
#include"Game/Boss/Behavior/IBossBehavior.h"

//ボスの棒立ちor試案処理
class BossIdle :public IBossBehavior{
public: //**パブリック関数
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="boss">ボスのポインタ</param>
	BossIdle(Boss* boss);
	~BossIdle() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

private: //**プライベート関数**//
	//ボスポインタ
	Boss* boss_;

	//idleパラメータ群
	struct Data
	{
		//アニメーション遷移時間
		float animeTransitionSec = 1.0f;

		//待機時間
		float idleSec = 3.0f;
		//待機カウント
		float currentSec = 0.0f;

	}data_;


};