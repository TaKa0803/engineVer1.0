#pragma once
#include<string>

//シーン
enum class SCENE {
	Debug,
	TITLE,
	GAME,
	GAMEOVER,
	GAMECLEAR,
	SceneCount	//使うな
};

//シーン基底クラス
class IScene {
protected://**共通変数**//

	//シーン番号
	static int sceneNo;
	//trueでゲーム終了
	static bool leaveGame;

public://**パブリック関数**//

	//仮想クラス
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual ~IScene();

	/// <summary>
	/// シーン番号取得
	/// </summary>
	/// <returns>シーン番号</returns>
	static int&GetSceneNo();

	//シーンセット
	static void SetScene(SCENE scene);

	//ゲーム終了フラグがONになっているか
	static bool GetLeaveScene() { return leaveGame; };

public://**パブリック変数**//

	//シーン名
	std::string name_="";
};

