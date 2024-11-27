#pragma once

#include "IScene/IScene.h"

#include<iostream>
#include<list>

#include"Input/Input.h"
#include"Camera/Camera.h"

#include"Game/Boss/Boss.h"
#include"Game/Player/Player.h"
#include"Game/Plane/Plane.h"
#include"Sprite/Sprite.h"

#include"ParticleData/ParticleManager.h"
#include"Game/FollowCamera/FollowCamera.h"
#include"Game/GameUI/GameUI.h"

class GameScene : public IScene {

public:

	//コンストラクタ
	GameScene();
	~GameScene()=default;

	//初期化処理
	void Initialize() override;
	//更新処理
	void Update() override;
	//描画処理
	void Draw() override;

private:
	//当たり判定処理
	void Collision();
	//シーン変更処理
	void SceneChange();

private:

	//キー入力
	Input* input_ = nullptr;

	//カメラ
	Camera* camera_;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	//プレイヤー
	std::unique_ptr<Player>player_;

	//ボス
	std::unique_ptr<Boss> boss_;

	//UI
	std::unique_ptr<GameUI>gameUI_;

	//地面
	std::unique_ptr<Plane>plane_;

	//パーティクルマネージャー
	std::unique_ptr<ParticleManager>particleM_;

	//シーンチェンジ用
	std::string white = "resources/Texture/SystemResources/white.png";
	//シーン遷移の黒色スプライト
	std::unique_ptr<Sprite>sceneC_;

	//シーン転換後の遷移処理完了フラグ
	bool preSceneChange_ = false;

	//シーン遷移処理をするか否か
	bool isSceneChange_ = false;

	//遷移までにかかる最大カウント
	const float maxSceneChangeCount_ = 60;
	//カウント
	float currentSceneXhangeCount_ = 0;

	//ゲーム内BGMの配列番号
	int bgmGame_;

	//画面シェイク判定
	bool isShake_ = false;


};