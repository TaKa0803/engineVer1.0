#pragma once
#include"IScene/IScene.h"
#include"Sprite/Sprite.h"
#include"Input/Input.h"

class GameOverScene :public IScene {

public:

	GameOverScene();

	~GameOverScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:
	//
	Input* inp_;

	//ロゴ
	std::unique_ptr<Sprite> logo_;
	
};