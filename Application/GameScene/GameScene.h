#pragma once
#include"SceneManager/SceneManager.h"
#include"SingleGameObjects/Model.h"
class GameScene {

public:

	GameScene();
	~GameScene();

	//初期化
	void Initialize();

	//ゲーム更新
	void Update();

	//ポストエフェクト描画
	void PostEffectDraw();

	//描画
	void Draw();

private:
	//シーンマネージャ
	SceneManager* sceneManager = nullptr;
	
};