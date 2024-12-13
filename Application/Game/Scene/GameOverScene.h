#pragma once
#include"IScene/IScene.h"
#include"Sprite/Sprite.h"
#include"Input/Input.h"

//ゲームオーバー時のシーン
class GameOverScene :public IScene {

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameOverScene();
	~GameOverScene()=default;

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
	Input* inp_;

	//ロゴスプライト
	std::unique_ptr<Sprite> logo_;
};