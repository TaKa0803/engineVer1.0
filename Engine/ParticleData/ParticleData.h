#pragma once
#include"Matrix.h"
#include"Vector4.h"
#include"struct.h"

struct PerFrame {
	float time;
	float deltaTime;
};

//円範囲のエミッター
struct EmiterSphere {
	Vector3 translate;		//生成中心座標
	float radius;			//生成範囲
	uint32_t  count;		//生成間隔
	float frequency;
	float frequencyTime;
	uint32_t emit;			//射出許可

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