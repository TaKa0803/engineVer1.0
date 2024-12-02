#pragma once
#include"3d/InstancingGameObject/InstancingGameObject.h"

#include<list>

class  EffectLargeDebris : public InstancingGameObject {

public:
	EffectLargeDebris();

	void Init();

	void Update();

	void Draw();

	void Spawn(const Vector3& pos);

	
public:

	//エフェクトのパラメータ
	struct  Parameter {

		//ワールド座標
		EulerWorldTransform world{};

		//速度
		Vector3 velo{};

		//加速度
		Vector3 acce{};

		float liveCount = 0;

		Vector4 color{};

		bool isDead = false;
	};

	std::list<Parameter>effects_;

public://パラメータ

	//出現範囲
	Vector2 spawnWide_ = { 5.0f, 15.0f};

	//初速度
	Vector2 spd_ = { 10.0f,40.0f };

	//重力
	float gravity_ = 10.0f;

	//同時生成数
	Vector2 spawnSameTimeNum_ = {5,10};

	//生存時間
	float liveSec_ = 3.0f;

	//エフェクトの色1
	Vector4 color1_{};

	//エフェクトの色2
	Vector4 color2_{};

	//エフェクトの色3
	Vector4 color3_{};
};