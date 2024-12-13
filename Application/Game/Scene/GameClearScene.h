#pragma once
#include"IScene/IScene.h"
#include"Sprite/Sprite.h"
#include"Input/Input.h"

//ゲームのクリアシーン
class GameClearScene :public IScene {

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameClearScene();
	~GameClearScene()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private: //**プライベート変数**//

	//入力
	Input* inp_=nullptr;

	//討伐時の画面の状態
	std::unique_ptr<Sprite> screanShot_;

	//クリア文字
	std::unique_ptr<Sprite>clearText_;

	//クリア文字
	std::unique_ptr<Sprite>bButton_;

	//クリアの音
	int bgmClear_;

};