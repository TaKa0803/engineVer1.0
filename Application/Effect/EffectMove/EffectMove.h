#pragma once
#include"ParticleManager.h"

class EffectMove {

public:

	EffectMove();
	~EffectMove();

	void Initialize(const Vector4& color);

	void Update();

	void Draw();

	void SpawnE(const Vector3& pos);
private:

	std::unique_ptr<ParticleManager>particleM_;
};