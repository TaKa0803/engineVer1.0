#include "CirccleShadow.h"

CirccleShadow::CirccleShadow(const EulerWorldTransform& world)
{
	InstancingGameObject::Initialize("DZone");

	SetParent(&world);
	SetColor({ 0,0,0,1 });
	SetTranslate({ 0,0.01f,0 });
	SetScale(1.5f);
}


