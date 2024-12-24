#include "CirccleShadow.h"

CirccleShadow::CirccleShadow(const EulerWorldTransform& world)
{
	//オブジェクト初期化
	InstancingGameObject::Initialize("DZone");

	//SetParent(&world);
	//ペアレント設定
	pworld_ = &world;
	//色設定
	SetColor(color_);
	//座標設定
	world_.translate_ = pos_;
	//サイズ設定
	SetScale(scale_);
}

void CirccleShadow::Update()
{
	//ターゲットの座標を取得
	world_.translate_= pworld_->translate_;
	world_.translate_.y = 0.01f;
	//オブジェクト更新
	InstancingGameObject::Update();
}

void CirccleShadow::SetShadowScale(float scale)
{
	//サイズ設定
	SetScale(scale);
}


