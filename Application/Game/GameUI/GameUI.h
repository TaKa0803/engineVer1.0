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

	//待機アイコン
	Vector3 waitATKpos = { 1000,622,0 };
	Vector3 waitATKscale = { 90,90 };

	//現在アイコン
	Vector3 ATKpos = { 1085,560 };
	Vector3 ATKscale = { 200,200 };

private:
	std::unique_ptr<Sprite>BButton_;
	std::unique_ptr<Sprite>punchSp_;

};