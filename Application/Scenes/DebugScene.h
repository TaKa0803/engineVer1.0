#pragma once

#include "IScene/IScene.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include<iostream>
#include"SphereCollider/SphereCollider.h"
#include"ColliderPlane/PlaneCollider.h"
#include"ColliderOBB/OBBCollider.h"

#include"Effect/EffectExplosion.h"
#include"2d/Sprite/Sprite.h"
#include"TextureManager/TextureManager.h"

class DebugScene : public IScene {

public:

	DebugScene();

	~DebugScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:

	void Debug();

private:
	//キー入力
	Input* input_ = nullptr;

	EulerWorldTransform cWorld_;
	Camera* camera_;

	
	EulerWorldTransform sw1;
	EulerWorldTransform sw2;
	EulerWorldTransform obbw;


	std::unique_ptr<SphereCollider> sp1;
	std::unique_ptr<SphereCollider> sp2;

	std::unique_ptr<OBBCollider> obb;

	std::unique_ptr<Sprite>splite_;

	//エフェクト
	EffectExplosion* EffectExp_;

	//エフェクト
	std::string eTag_ = "box";

	float spd_ = 0.5;
};