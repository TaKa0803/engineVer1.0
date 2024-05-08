#pragma once
#include"Vector3.h"
#include"InstancingGameObject/InstancingGameObject.h"

#include<list>



class EffectImpact : public InstancingGameObject {

public:

	void Initialize();

	void Update();

	void Draw();

	void Spawn(const EulerWorldTransform playerW);
private:
	struct ImpactData
	{
		EulerWorldTransform base;

		EulerWorldTransform center;

		EulerWorldTransform pos;

		Vector3 velo;

		float milit=60.0f;
	};


	std::list<ImpactData*>datas_;

	int spawnNum = 10;
};