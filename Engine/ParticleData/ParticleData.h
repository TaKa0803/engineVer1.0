#pragma once
#include"Matrix.h"
#include"Vector4.h"
#include"Vector2.h"
#include"struct.h"

struct PerFrame {
	float time;
	float deltaTime;
};

//円範囲のエミッター
struct EmiterSphere {

	Vector4 color;

	//float padd;

	Vector3 translate;		//生成中心座標

	float pad;

	Vector2 radius;			//生成範囲
	Vector2 count;			//生成量


	Vector2 speed;			//最小最大速度
	Vector2 veloX;			//
	Vector2 veloY;
	Vector2 veloZ;

	float emit;			//射出許可

	float frequency;		//生成間隔
	
	float frequencyTime;	//生成時間カウント

	float lifetime;


};

//パーティクル本体のデータ
struct Particle {
	int32_t isActive;	//有効フラグ
	Vector3 translate;	//座標
	Vector3 scale;		//サイズ
	float lifeTime;		//生存時間
	Vector3 velocity;	//移動速度
	float currentTime;  //更新秒数
	Vector4 color;		//色
};

struct PerView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
	float deltaTime;
};