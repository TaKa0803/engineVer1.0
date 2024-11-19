#pragma once
#include"IScene/IScene.h"
#include"Sprite/Sprite.h"
#include"Input/Input.h"

class GameClearScene :public IScene {

public:

	GameClearScene();

	~GameClearScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:
	//
	Input* inp_=nullptr;

	//ロゴ
	std::unique_ptr<Sprite> logo_;

};