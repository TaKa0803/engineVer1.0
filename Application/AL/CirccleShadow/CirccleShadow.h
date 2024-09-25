#pragma once
#include"InstancingGameObject/InstancingGameObject.h"

class CirccleShadow : public InstancingGameObject {

public:
	CirccleShadow(const EulerWorldTransform& world);
	~CirccleShadow()=default;
};
