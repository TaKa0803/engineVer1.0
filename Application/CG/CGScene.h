#pragma once

#include "IScene/IScene.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SingleGameObjects/GameObject.h"
#include"SkyBoxModel/SkyBoxModel.h"
#include"ParticleData/ParticleManager.h"
#include"GVariableManager/GVaribleManager.h"


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

	GlobalVariableManager* agvM_;

	Camera* camera_;

	std::unique_ptr<GameObject>object;

	GameObject*terrain;

	SkyBoxModel* skybox_;

	PointLight pointLight_;

	DirectionalLight dLight_;

	int ball;

	std::unique_ptr<ParticleManager>particleManager_;
};