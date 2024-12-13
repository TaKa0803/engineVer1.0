#pragma once
#include"WorldTransform/WorldTransform.h"
#include"ColliderOBB/OBBCollider.h"
#include"SphereCollider/SphereCollider.h"
#include"struct.h"

//カメラ処理
class Camera {

public: //**シングルトンパターン**//

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Camera* GetInstance();
private:
	Camera() = default;
	~Camera() = default;
	Camera(const Camera& o) = delete;
	const Camera& operator=(const Camera& o) = delete;

public: //**パブリック変数**//

	//カメラ本体のワールド
	EulerWorldTransform mainCamera_;

public: //**パブリック関数**//

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// マトリックス更新
	/// </summary>
	void UpdateMatrixes();

	/// <summary>
	/// カメラとOBBの判定を取って埋まらないようにする処理
	/// </summary>
	/// <param name="obb">コライダー</param>
	void IsCollision(OBBCollider*obb);

public: //**ゲッター**//

	/// <summary>
	/// カメラが注目しているものの座標取得
	/// </summary>
	/// <returns>translateを返却</returns>
	const Vector3& GetFeaturedPos()const { return CameraMotionSupport_.translate_; }

	/// <summary>
	/// ポイントカメラの親のワールド取得
	/// </summary>
	/// <returns></returns>
	const EulerWorldTransform& GetFeaturedWorld()const { return *FeaturedWorldTransform_; }

	/// <summary>
	/// メインカメラのWorldTransform取得
	/// </summary>
	/// <returns>WorldTransformを返却</returns>
	const EulerWorldTransform& GetMainCamera()const { return mainCamera_; }

	/// <summary>
	/// ViewProjectionを取得
	/// </summary>
	/// <returns>ViewProjectionの4x4行列を返却</returns>
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjection_; }

	/// <summary>
	/// ビューポート取得
	/// </summary>
	/// <returns>ビューポートの行列</returns>
	const Matrix4x4& GetViewportMatrix()const { return viewPort_; }

	/// <summary>
	/// プロジェクション行列取得
	/// </summary>
	/// <returns>プロジェクション行列</returns>
	const Matrix4x4& GetProjectionMatrix()const { return projection_; }

	/// <summary>
	/// ポイントカメラ時の回転量Yを取得
	/// </summary>
	/// <returns>回転量Y</returns>
	float GetPCameraR_Y() { return CameraMotionSupport_.rotate_.y; }

	/// <summary>
	/// カメラの視線取得
	/// </summary>
	/// <returns></returns>
	const Segment GetSegment() { return segment_; }

public: //**セッター**//

	/// <summary>
	/// カメラの追従ターゲット設定
	/// </summary>
	/// <param name="parent">追従座標</param>
	void SetTarget(const EulerWorldTransform* parent) { FeaturedWorldTransform_ = parent; };

	/// <summary>
	/// カメラのターゲットとの距離設定
	/// </summary>
	/// <param name="far">距離</param>
	void SetCameraDirection(const float far);

	/// <summary>
	/// メインカメラの座標設定
	/// </summary>
	/// <param name="pos">座標</param>
	void SetMainCameraPos(const Vector3& pos) { camerapos_ = pos; }

	/// <summary>
	/// ポイントカメラの基点の座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetCameraFeaturedPos(const Vector3& pos) { CameraMotionSupport_.translate_ = pos; }

	/// <summary>
	/// ポイントカメラの回転量Yに値を追加
	/// </summary>
	/// <param name="y">追加する回転量Y</param>
	void AddCameraR_Y(float y) { CameraMotionSupport_.rotate_.y += y; }

	/// <summary>
	/// ポイントカメラの回転量Xに値を追加
	/// </summary>
	/// <param name="x">追加する回転量X</param>
	void AddCameraR_X(float x) { CameraMotionSupport_.rotate_.x += x; }

	/// <summary>
	/// カメラからみたベクトル方向に変換
	/// </summary>
	/// <param name="velo">元のベクトル</param>
	/// <returns>変換したベクトル</returns>
	Vector3 SetDirection4Camera(const Vector3&velo)const;

private: //**プライベート変数**//

	//カメラの平行移動
	Vector3 camerapos_{};

	//カメラの注目点があるときの回転処理用
	EulerWorldTransform CameraMotionSupport_;

	//注目しているWorldT
	const EulerWorldTransform* FeaturedWorldTransform_ = nullptr;

	//カメラの注目品との初期距離
	const float rangeCameraFeaturedPoint = -50;

	//完全追従するか座標のみ取得するか
	bool isOnlyGetPosition = true;

	//描画距離
	float FarZ = 1000.0f;

	//ビュー
	Matrix4x4 view_ = MakeIdentity4x4();

	//プロジェクション
	Matrix4x4 projection_ = MakeIdentity4x4();

	//ビュープロジェクション
	Matrix4x4 viewProjection_ = MakeIdentity4x4();

	//ビューポート
	Matrix4x4 viewPort_ = MakeIdentity4x4();

	//注目点都の値
	float farFeaturedPos_ = -50;

	//カメラの視線
	Segment segment_;

	//あたった時の追加で寄せる量
	float direction = 0;

	//カメラのオブジェクト当たり判定
	bool isCollision_ = false;
};