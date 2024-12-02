#pragma once
#include"Sprite/Sprite.h"



class GameUI {

public:
	GameUI();
	~GameUI()=default;

	void Initialize();

	void Update();

	void Draw();
private:


private:
	//Bボタンアイコン
	std::unique_ptr<Sprite>BButton_;
	std::unique_ptr<Sprite>AButton_;
	std::unique_ptr<Sprite>XButton_;


	//攻撃ボタンアイコン
	std::unique_ptr<Sprite>punchSp_;
	std::unique_ptr<Sprite>dashSp_;
	std::unique_ptr<Sprite>rollSp_;

};