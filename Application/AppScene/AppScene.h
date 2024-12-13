#pragma once
#include"SceneManager/SceneManager.h"
#include"SingleGameObjects/Model.h"

//アプリ層の最下層シーン
class AppScene {

public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	AppScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~AppScene()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: //**プライベート変数**//

	//シーンマネージャ
	std::unique_ptr<SceneManager> sceneManager = nullptr;
	
};