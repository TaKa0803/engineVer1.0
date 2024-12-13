#pragma once
#include"InstancingGameObject/InstancingGameObject.h"

//円の影
class CirccleShadow : public InstancingGameObject {

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="world">ターゲット座標</param>
	CirccleShadow(const EulerWorldTransform& world);
	~CirccleShadow()=default;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 影のサイズ設定
	/// </summary>
	/// <param name="scale"></param>
	void SetShadowScale(float scale);

private: //**プライベート変数**//

	//ターゲットのワールド
	const EulerWorldTransform* pworld_;

	//色
	Vector4 color_ = { 0,0,0,1 };

	//座標
	Vector3 pos_ = { 0,0.01f,0 };

	//サイズ
	float scale_ = 1.5f;
};
