#pragma once
#include"Game/Boss/Behavior/IBossBehavior.h"


//様子見移動
class BossDown :public IBossBehavior{
public:

	BossDown(Boss*boss);
	~BossDown() = default;

	void Initialize()override;

	void Update()override;


private:

	Boss* boss_;

	//アニメーション時間
	float animeSec_ = 1.0f;

	//次に行くまでの時間
	float nextSec_ = 1.0f;

	//カウント
	float currentNext_ = 0;
private://デバッグ

};