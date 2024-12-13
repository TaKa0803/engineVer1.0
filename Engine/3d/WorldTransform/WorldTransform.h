#pragma once
//#include"struct.h"
#include"Math/Matrix.h"
#include"Quaternion.h"
#include"GvariGroup/GvariGroup.h"

class EulerWorldTransform {
public://変数

	//座標
	Vector3 translate_ = { 0.0f,0.0f,0.0f };

	//回転
	Vector3 rotate_ = { 0.0f,0.0f,0.0f };

	//拡縮
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//ワールド行列
	Matrix4x4 matWorld_ = MakeIdentity4x4();

	//親
	const EulerWorldTransform* parent_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	Matrix4x4 UpdateMatrix();
#pragma region ゲッター
public://Getter

	/// <summary>
	/// matWorldのTranslate取得
	/// </summary>
	/// <returns>ワールドのtranslate</returns>
	const Vector3 GetWorldTranslate() const {
		Vector3 matTranslation = {
			matWorld_.m[3][0],
			matWorld_.m[3][1],
			matWorld_.m[3][2]
		};
		return matTranslation;
	};

	GlobalVariableTree& GetDebugTree(const std::string& name = "ワールド");

	GlobalVariableTree& GetDebugMonitorTree(const std::string& name = "ワールド");

public:
	/// <summary>
	/// すべて同じ値でサイズ設定
	/// </summary>
	/// <param name="radius"></param>
	void SetScale(float radius) { scale_ = { radius,radius,radius }; }

	void SetScale(Vector3 scale) { scale_ = scale; }

#pragma endregion
	
	//デバッグ用ツリー
	GlobalVariableTree tree_ = GlobalVariableTree("");

};


class QuaterinionWorldTransform {
public://変数

	//座標
	Vector3 translate_ = { 0.0f,0.0f,0.0f };

	//回転
	Quaternion rotate_ = { 0.0f,0.0f,0.0f };

	//拡縮
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//ワールド行列
	Matrix4x4 matWorld_ = MakeIdentity4x4();

	//親
	const QuaterinionWorldTransform* parent_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	Matrix4x4 UpdateMatrix();

	/// <summary>
	/// パラメータデバッグ表示
	/// </summary>
	//void DrawDebug(const char* name);


#pragma region ゲッター
public://Getter

	/// <summary>
	/// matWorldのTranslate取得
	/// </summary>
	/// <returns>ワールドのtranslate</returns>
	const Vector3 GetMatWorldTranslate()  {
		UpdateMatrix();
		Vector3 matTranslation = {
			matWorld_.m[3][0],
			matWorld_.m[3][1],
			matWorld_.m[3][2]
		};
		return matTranslation;
	}

public:
	/// <summary>
	/// すべて同じ値でサイズ設定
	/// </summary>
	/// <param name="radius"></param>
	void SetScale(float radius) { scale_ = { radius,radius,radius }; }

	void SetScale(Vector3 scale) { scale_ = scale; }
#pragma endregion

private:



};