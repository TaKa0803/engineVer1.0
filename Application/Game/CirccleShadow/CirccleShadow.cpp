#include "CirccleShadow.h"

CirccleShadow::CirccleShadow(const EulerWorldTransform& world)
{
	InstancingGameObject::Initialize("DZone");

	//SetParent(&world);
	pworld_ = &world;

	SetColor({ 0,0,0,1 });
	SetTranslate({ 0,0.01f,0 });
	SetScale(1.5f);
}

void CirccleShadow::Update()
{
	world_.translate_= pworld_->translate_;
	world_.translate_.y = 0.01f;
	InstancingGameObject::Update();
}

void CirccleShadow::SetShadowScale(float scale)
{
	SetScale(scale);
}


