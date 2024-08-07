#pragma once
#include"Matrix.h"
#include"Vector4.h"
#include"struct.h"

struct PerFrame {
	float time;
	float deltaTime;
};

struct EmiterSphere {
	Vector3 translate;
	float radius;
	uint32_t  count;
	float frequency;
	float frequencyTime;
	uint32_t emit; //射出許可

};

struct Particle {
	int32_t isActive;
	Vector3 translate;
	Vector3 scale;
	float lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};

struct PerView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
	float deltaTime;
};