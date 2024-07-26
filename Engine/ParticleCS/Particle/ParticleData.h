#pragma once
#include"Matrix.h"
#include"Vector4.h"
#include"struct.h"


struct Particle {
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
};