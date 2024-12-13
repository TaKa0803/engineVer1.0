#pragma once
#include"Camera/Camera.h"
#include"Input/Input.h"

//追従カメラクラス
class FollowCamera {
public:	//**パブリック関数**//
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="playerWorld">プレイヤーワールド</param>
	/// <param name="target">ターゲット</param>
	FollowCamera(const EulerWorldTransform* playerWorld,const  EulerWorldTransform* target);
	~FollowCamera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="isShake">揺れるかのフラグ</param>
	void Update(bool& isShake);

private: //**プライベート関数**//

	/// <summary>
	/// ボスの方向を見る処理
	/// </summary>
	void SetFocusBoss();
private: //**参照物**//

	//入力
	Input* input_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//プレイヤーワールド
	const EulerWorldTransform* player_;

	//ボスのワールド
	const EulerWorldTransform* boss_;

private: //**パラメータ変数**//

	//カメラの最大回転量
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

	//カメラシェイクの時間カウント
	int cameraShakeCount_ = 0;
	Vector2 shakenum = { 0,0 };
	Vector3 tempP_;


};