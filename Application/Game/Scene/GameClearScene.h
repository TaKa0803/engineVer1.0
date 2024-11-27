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
	//入力
	Input* inp_=nullptr;

	//討伐時の画面の状態
	std::unique_ptr<Sprite> screanShot_;

	//クリア文字
	std::unique_ptr<Sprite>clearText_;

	//クリア文字
	std::unique_ptr<Sprite>bButton_;

};