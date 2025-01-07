#pragma once
#include"IScene/IScene.h"

#pragma region 各シーンクラス
#include"Debug/DebugScene.h"
#include"Game/Scene/TitleScene.h" 
#include"Game/Scene/InGameScene.h" 
#include"Game/Scene/GameClearScene.h" 
#include"Game/Scene/GameOverScene.h" 
#pragma endregion

#include<iostream>

//**現在未使用。エンジンの調整後利用する。

//番号に合ったシーンの生成処理
class SceneFactory {

public://**パブリック関数**//
	
	/// <summary>
	/// 指定したシーンクラスを読み込む
	/// </summary>
	/// <param name="scene">シーン名</param>
	/// <returns>シーンのユニークポインタ</returns>
	static std::unique_ptr<IScene>CreateScene(SCENE scene) {
		switch (scene)
		{
		case SCENE::Debug:
			return std::make_unique<DebugScnene>();

		case SCENE::TITLE:
			return std::make_unique<TitleScene>();

		case SCENE::GAME:
			return std::make_unique<InGameScene>();

		case SCENE::GAMEOVER:
			return std::make_unique<GameOverScene>();

		case SCENE::GAMECLEAR:
			return std::make_unique<GameClearScene>();

		case SCENE::SceneCount:
			return nullptr;

		default:
			return nullptr;
		}
	}

};