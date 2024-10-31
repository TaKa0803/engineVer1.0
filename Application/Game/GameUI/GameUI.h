#pragma once
#include"Sprite/Sprite.h"



class GameUI {

public:
	GameUI();
	~GameUI()=default;

	void Initialize();

	void Update();

	void DrawGame();

	void DrawClear();


	void SetBState(int state);
public:

	enum ButtonState {
		Wait,
		Punch,
		Kick,
		Ult
	};

private:

	//待機アイコン
	Vector3 waitATKpos = { 1000,622,0 };
	Vector3 waitATKscale = { 90,90 };

	//現在アイコン
	Vector3 ATKpos = { 1085,560 };
	Vector3 ATKscale = { 200,200 };

private:

	ButtonState BState_=Wait;

	std::unique_ptr<Sprite>skillSp_;
	std::unique_ptr<Sprite>BButton_;
	std::unique_ptr<Sprite>punchSp_;
	std::unique_ptr<Sprite>kickSp_;
	std::unique_ptr<Sprite>ultSp_;


	std::unique_ptr<Sprite>backScreen_;
	std::unique_ptr<Sprite>resultText_;

};