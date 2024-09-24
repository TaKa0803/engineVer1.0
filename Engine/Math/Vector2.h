#pragma once

class Vector2 {
public:
	float x;
	float y;	

	Vector2 Normalize();

	Vector2& operator+(Vector2 v) { x += v.x; y += v.y; return *this; }
	Vector2& operator-(Vector2 v) { x -= v.x; y -= v.y; return *this; }
	Vector2& operator*(Vector2 v) { x *= v.x; y *= v.y; return *this; }

	friend Vector2 operator+(const Vector2& a, const Vector2& b) { return Vector2( a.x + b.x,a.y +b.y ); }

	friend Vector2 operator-(const Vector2& a, const Vector2& b) { return Vector2(a.x - b.x, a.y - b.y); }

	friend float operator*(const Vector2& a, const Vector2& b) { return a.x * b.x+ a.y * b.y; }

	friend bool operator==(const Vector2& v1, const Vector2& v2) {
		if (v1.x == v2.x && v1.y == v2.y) { return true; }
		return false;
	}

	Vector2& operator+=(Vector2 v) { x += v.x; y += v.y; return *this; }
	Vector2& operator-=(Vector2 v) { x -= v.x; y -= v.y; return *this; }
	Vector2& operator*=(float s) { x *= s; y *= s; return *this; }
	Vector2& operator/=(float s) { x /= s; y /= s; return *this; }

};

// 長さ
float Length(const Vector2& v);

Vector2 Normalize(const Vector2& v);

float Dot(const Vector2& v1, const Vector2& v2);

