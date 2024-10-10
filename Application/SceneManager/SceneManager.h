#pragma once
#include"IScene/IScene.h"
#include"SingleGameObjects/Model.h"

#include<iostream>
#include<vector>

/// <summary>
/// シーン管理マネージャー
/// </summary>
class SceneManager {

public:

	SceneManager();
	~SceneManager();

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
	
private:

	/// <summary>
	/// デバッグウィンドウ表示
	/// </summary>
	void DebugWindow();

private:

	/// <summary>
	/// 各種シーン管理
	/// </summary>
	std::vector<std::unique_ptr<IScene>> sceneArr_;

	/// <summary>
	/// シーンの名前
	/// </summary>
	std::vector<std::string> sceneName_;


	///呼び出すステージの管理
	//現在のシーン
	int currentSceneNo_=0;
	//1F前のシーン
	int prevSceneNo_=-1;


};