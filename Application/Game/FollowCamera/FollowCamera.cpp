#include "FollowCamera.h"
#include"RandomNum/RandomNum.h"
#include"GvariGroup/GvariGroup.h"

#pragma region 三次元ベクトルの回転
// 正面を {0, 0, 1} としたときの方向ベクトル `velo` への回転量を求める関数
Vector3 rotationToDirection(const Vector3&offset,const Vector3& velo) {
	Vector3 zAxis=offset;      // 正面方向の基準ベクトル
	Vector3 d = velo.GetNormalizeNum();         // 方向ベクトルを正規化

	// 各軸まわりの回転量を計算
	float pitch = std::asin(-d.y);               // y軸まわりの回転（ピッチ）
	float yaw = std::atan2(d.x, d.z);            // z軸まわりの回転（ヨー）
	float roll = 0.0f;                           // x軸まわりの回転（ロール）: 常に 0 と仮定

	// 結果の回転量を返す（ピッチ、ヨー、ロール）
	return Vector3(pitch, yaw, roll);
}
#pragma endregion


FollowCamera::FollowCamera(const EulerWorldTransform* world, const EulerWorldTransform* target)
{
	input_ = Input::GetInstance();
	//中身設定
	camera_ = Camera::GetInstance();
	//ペアレント元保存
	player_ = world;

	boss_ = target;

	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("追従カメラ");
	gvg->SetValue("X軸のカメラ回転最大制限", &rotateLimitMax);
	gvg->SetValue("X軸のカメラ回転最小制限", &rotateLimitMin);

	gvg->SetValue("カメラの高さ", &cameraHeight_);
	gvg->SetValue("ターゲット位置との距離", &length_);

	gvg->SetValue("ボス座標のずれの値", &bDiffPos_);
	gvg->SetValue("プレイヤー座標のずれの値", &pDiffPos_);
	gvg->SetTreeData(camera_->mainCamera_.GetDebugTree());
}

void FollowCamera::Initialize()
{
	//カメラ初期化
	camera_->Initialize();
	//ペアレント
	//camera_->SetTarget(parent_);

	//各種設定
	camera_->SetCameraDirection(-40.0f);
	
}

void FollowCamera::Update(bool& isShake)
{


	//カメラ位置設定
	Vector3 cPos = player_->GetWorldTranslate();

	//y軸なしのボス、プレイヤー向き取得
	Vector3 bpos = boss_->GetWorldTranslate();
	Vector3 ppos = player_->GetWorldTranslate();
	bpos.y = 0;
	ppos.y = 0;

	//ボスと反対方向の向きベクトル(X,Z)追加
	Vector3 direc = bpos - ppos;
	direc *= -1;
	direc.y = 0;
	direc.SetNormalize();
	
	//距離分話した座標をカメラに追加
	cPos += direc * length_;
	cPos.y = cameraHeight_;

	camera_->mainCamera_.translate_ = cPos+pDiffPos_;

	//カメラ
	SetFocusBoss();

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



void FollowCamera::SetFocusBoss()
{
	//ボス方向にカメラを向ける
	//向きベクトル取得
	camera_->UpdateMatrixes();
	Vector3 direc = (boss_->GetWorldTranslate()+bDiffPos_) - camera_->GetMainCamera().GetWorldTranslate();
	
	//もしカメラと同座標なら処理を行わない
	if (direc.GetLength() == 0.0f) {
		return;
	}
	
	//カメラの向き取得
	Vector3 offset = { 0,0,1 };
	offset = camera_->SetDirection4Camera(offset);

	Vector3 rotate = rotationToDirection(offset, direc);

	if (rotate.x > rotateLimitMax) {
		rotate.x = rotateLimitMax;
	}
	else if (rotate.x < rotateLimitMin) {
		rotate.x = rotateLimitMin;
	}

	camera_->mainCamera_.rotate_ = rotate;
}
