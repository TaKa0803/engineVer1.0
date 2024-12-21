#pragma once
#include"Matrix.h"
#include"Vector4.h"
#include"Vector2.h"
#include"struct.h"

struct PerFrame {
	//時間
	float time;
	//デルタタイム
	float deltaTime;
};

//円範囲のエミッター
struct EmiterSphere {
	//色
	Vector4 color;

	//float padd;

	Vector3 translate;		//生成中心座標

	float pad;

	Vector2 radius;			//生成範囲
	Vector2 count;			//生成量


	Vector2 speed;			//最小最大速度
	Vector2 veloX;			//最小最大向き方向X
	Vector2 veloY;			//最小最大向き方向Y
	Vector2 veloZ;			//最小最大向き方向Z

	float emit;			//射出許可

	float frequency;		//生成間隔
	
	float frequencyTime;	//生成時間カウント

	float lifetime;			//生成時間


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
	//ビュープロジェクション
	Matrix4x4 viewProjection;
	//ビルボードマトリックス
	Matrix4x4 billboardMatrix;
	//デルタタイム
	float deltaTime;
};