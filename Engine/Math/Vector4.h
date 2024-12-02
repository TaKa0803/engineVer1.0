#pragma once

struct Vector4 {
	float x = 1;
	float y = 1;
	float z = 1;
	float w = 1;

	
	friend Vector4 operator-(const Vector4& v) { return { -v.x,-v.y,-v.z,-v.w }; }
};

Vector4 Lerp(const Vector4& st, const Vector4& ed, float t);