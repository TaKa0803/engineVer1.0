#pragma once

#include "IScene/IScene.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SingleGameObjects/GameObject.h"
#include"SkyBoxModel/SkyBoxModel.h"

class CGScnene : public IScene {

public:

	CGScnene();
	~CGScnene();

	void Initialize() override;
	void Update() override;

	void PostEffectDraw()override;

	void Draw() override;

private:

	void Debug();

	//キー入力
	Input* input_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<GameObject>object;

	GameObject*terrain;

	SkyBoxModel* skybox_;

	PointLight pointLight_{ 0,2,0 };



	int ball;
};