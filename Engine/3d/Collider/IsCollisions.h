#pragma once
#include"WorldTransform/WorldTransform.h"

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct AABB {
	Vector3 minV;
	Vector3 maxV;
};

struct Sphere {
	Vector3 center;
	float radius;
};

//すべてのX回転量取得
float GetAllRotateX(const EulerWorldTransform& world);
//すべてのY回転量取得
float GetAllRotateY(const EulerWorldTransform& world);
//すべてのZ回転量取得
float GetAllRotateZ(const EulerWorldTransform& world);
//親元も合わせた回転量取得
Vector3 GetAllRotate(const EulerWorldTransform& world);

//すべてのX拡縮取得
float GetAllScaleX(const EulerWorldTransform& world);
//すべてのY拡縮取得
float GetAllScaleY(const EulerWorldTransform& world);
//すべてのZ拡縮取得
float GetAllScaleZ(const EulerWorldTransform& world);

//親も合わせた拡縮取得
Vector3 GetAllScale(const EulerWorldTransform& world);

Vector3 GetClosestPoint(const Vector3& p, const Vector3& min, const Vector3& max);

//AABBと円
bool InCollision(const AABB& a, const Sphere& s, Vector3& v);

bool InCollision(const AABB& AA, const Segment& S);