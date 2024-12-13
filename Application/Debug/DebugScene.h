#pragma once

#include "IScene/IScene.h"
#include"Input/Input.h"
#include"Camera/Camera.h"
#include"SingleGameObjects/GameObject.h"
#include"SkyBoxModel/SkyBoxModel.h"
#include"ParticleData/ParticleManager.h"

//　デバッグ用シーン
//  主に技術検証用のシーン
//　ゲームとは無関係
class DebugScnene : public IScene {

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugScnene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugScnene();

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

private: //**プライベート関数**//

	/// <summary>
	/// デバッグ処理
	/// </summary>
	void Debug();

private: //**プライベート変数**//

	//キー入力
	Input* input_ = nullptr;

	//カメラ
	Camera* camera_;

	//オブジェクト
	std::unique_ptr<GameObject>object;

	//山モデル
	GameObject*terrain;

	//スカイボックス
	SkyBoxModel* skybox_;

	//ポイントライト
	PointLight pointLight_;

	//ディレクショナルライト
	DirectionalLight dLight_;

	//パーティクルマネージャ
	std::unique_ptr<ParticleManager>particleManager_;
};