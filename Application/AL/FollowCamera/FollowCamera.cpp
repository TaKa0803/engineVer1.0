#include "FollowCamera.h"
#include"RandomNum/RandomNum.h"

FollowCamera::FollowCamera(const EulerWorldTransform* world)
{
	input_ = Input::GetInstance();
	//中身設定
	camera_ = Camera::GetInstance();
	//ペアレント元保存
	parent_ = world;
}

void FollowCamera::Initialize()
{
	//カメラ初期化
	camera_->Initialize();
	//ペアレント
	camera_->SetTarget(parent_);

	//各種設定
	camera_->SetCameraDirection(-40.0f);
	
}

void FollowCamera::Update(bool& isShake)
{
	//カメラ更新
	Vector2 stick{};

	//コントローラのアクセスチェック
	if (input_->IsControllerActive()) {
		stick += input_->GetjoyStickR();
	}
	
	Vector3 sti = input_->GetAllArrowKey();
	stick += Vector2{ sti.x,sti.z };
	
	//入力を正規化
	stick.Normalize();
	stick.x *= xrotateNum;
	stick.y *= yrotatenum * -1.0f;
	camera_->AddCameraR_X(stick.y);
	camera_->AddCameraR_Y(stick.x);



	//カメラ振動処理
	if (isShake) {

		Vector3 pos = {
		RandomNumber::Get(-0.5f,0.5f),
		RandomNumber::Get(-0.5f,0.5f),
		0
		};

		Vector3 newp = tempP_ + pos;

		camera_->SetMainCameraPos(newp);

		if (cameraShakeCount_-- <= 0) {
			isShake = false;
			cameraShakeCount_ = 0;
			camera_->SetMainCameraPos(tempP_);
		}

	}
}
