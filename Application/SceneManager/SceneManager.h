#pragma once
#include"IScene/IScene.h"
#include"SingleGameObjects/Model.h"

#include<iostream>
#include<vector>

/// <summary>
/// シーン管理マネージャー
/// </summary>
class SceneManager {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager();
	~SceneManager()=default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private://**プライベート変数**//

	/// <summary>
	/// シーン
	/// </summary>
	std::unique_ptr<IScene> sceneArr_;

	//現在のシーン
	std::string scenename_ = "";

	///呼び出すステージの管理
	//現在のシーン
	int currentSceneNo_ = 0;
	//1F前のシーン
	int prevSceneNo_ = -1;
};