#pragma once

#include "IScene/IScene.h"

#include<iostream>
#include<list>

#include"Input/Input.h"
#include"Camera/Camera.h"

#include"AL/ALBoss/ALBoss.h"
#include"AL/Player/ALPlayer.h"
#include"AL/Plane/Plane.h"
#include"AL/Enemy/ALEnemy.h"
#include"AL/EnemyPopManager/EnemyPopManager.h"
#include"Sprite/Sprite.h"
#include"AL/BrokenBody/BrokenBody.h"
#include"ParticleData/ParticleManager.h"
#include"AL/FollowCamera/FollowCamera.h"
#include"AL/GameUI/GameUI.h"

class ALGameScene : public IScene {

public:

	ALGameScene();

	~ALGameScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:
	void DebugWindows();

	void Collision();

	void SceneChange();



	//void ShakeStart(int count);


private:
	//キー入力
	Input* input_ = nullptr;
	//カメラクラス
	Camera* camera_;
	//カメラの処理
	std::unique_ptr<FollowCamera> followCamera_;

	///以下ゲーム実装
	std::unique_ptr<ALPlayer>player_;

	std::unique_ptr<ALBoss> boss_;

	std::unique_ptr<GameUI>gameUI_;

	std::unique_ptr<Plane>plane_;

	int limitMinute = 0;
	const int maxmilitMinute = 60 * 60;
	
	enum Scene {
		Game,
		Clear,
	};

	Scene scene_ = Game;

	GameUI::ButtonState BState_ = GameUI::Wait;




	float alphaNum_ = 0;

	const float addAlphaNum_ = 120.0f;

	const float screenmaxAlphaNum_ = 0.7f;


	//1フレームごとに計算する敵の数
	const float frameCountEnemy_ = 1.0f;

	float nowFrameCountEnemy_ = 0;

	bool serchComplete_ = false;


	BrokenBody* brokenBody_;

	//シーンチェンジ用
	std::string white = "resources/Texture/SystemResources/white.png";
	std::unique_ptr<Sprite>sceneC_;

	//シーン転換後の処置
	bool preSceneChange_ = false;

	//シーン転換処理をするか否か
	bool isSceneChange_ = false;

	const float maxSceneChangeCount_ = 60;

	float sceneXhangeCount_ = 0;

	int bgmGame_;

	int bgmClear_;

	bool isShake_ = false;


	std::unique_ptr<ParticleManager>peM_;
};