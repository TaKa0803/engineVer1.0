#pragma once
#include"Camera/Camera.h"
#include"Input/Input.h"

class FollowCamera {
public:

	FollowCamera(const EulerWorldTransform* worldconst,const  EulerWorldTransform* target);
	~FollowCamera() = default;

	void Initialize();

	void Update(bool& isShake);

private://関数



	void SetFocusBoss();
private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	const EulerWorldTransform* player_;

	const EulerWorldTransform* boss_;

private://パラメータ変数

	//カメラ本体の位置
	float rotateLimitMax = 1.0f;
	float rotateLimitMin = -1.0f;

	//カメラの高さ
	float cameraHeight_ = 1.0f;
	//プレイヤーとのXZ距離
	float length_ = 40;

	//カメラ移動の中心点
	EulerWorldTransform cameraPoint_{};

	//ボス座標のずれの値
	Vector3 bDiffPos_;
	//プレイヤー座標のずれの値
	Vector3 pDiffPos_;
private://メンバ変数

	int cameraShakeCount_ = 0;
	Vector2 shakenum = { 0,0 };
	Vector3 tempP_;


};