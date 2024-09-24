#pragma once
#include"Camera/Camera.h"
#include"Input/Input.h"

class FollowCamera {
public:

	FollowCamera(const EulerWorldTransform* world);
	~FollowCamera() = default;

	void Initialize();

	void Update(bool& isShake);

private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	const EulerWorldTransform* parent_;

private://パラメータ変数

	float xrotateNum = 0.05f;
	float yrotatenum = 0.02f;

private://メンバ変数

	int cameraShakeCount_ = 0;
	Vector2 shakenum = { 0,0 };
	Vector3 tempP_;
};