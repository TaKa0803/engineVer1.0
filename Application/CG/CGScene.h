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

	
	void Draw() override;

private:

	void Debug();

	//キー入力
	Input* input_ = nullptr;

	Camera* camera_;

	std::unique_ptr<GameObject>object;

	GameObject*terrain;

	SkyBoxModel* skybox_;

	PointLight pointLight_;

	DirectionalLight dLight_;

	int ball;
};