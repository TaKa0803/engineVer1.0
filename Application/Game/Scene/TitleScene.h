#pragma once
#include "IScene/IScene.h"
#include"Input/Input.h"
#include"Sprite/Sprite.h"

//タイトルシーン
class TitleScene : public IScene {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

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

private://**プライベート関数**//

	/// <summary>
	/// デバッグ処理
	/// </summary>
	void Debug();

	/// <summary>
	/// シーン変更処理
	/// </summary>
	void SceneChange();

private://**プライベート変数**//

	//キー入力
	Input* input_ = nullptr;

	//タイトル画像
	std::unique_ptr<Sprite> sprite_;

	//プレス画像
	std::unique_ptr<Sprite>pressSp_;

	//Bボタン画像
	std::unique_ptr<Sprite>BButtonSp_;

	//シーンチェンジ用
	std::string white = "resources/Texture/SystemResources/white.png";
	std::unique_ptr<Sprite>sceneC_;

	//シーン転換後の処置
	bool preSceneChange_ = false;

	//シーン転換処理をするか否か
	bool isSceneChange_ = false;

	//シーン変更カウント
	const float maxSceneChangeCount_ = 60;

	//カウント
	float sceneXhangeCount_ = 0;

	//点滅
	const int maxTenmetu = 30;
	int tenmetuCount = 0;
	bool isDrawB_ = true;

	//タイトルの音
	int titleSound_;
};