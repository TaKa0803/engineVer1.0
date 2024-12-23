#pragma once
#include"Sprite/Sprite.h"

//プレイヤー前方宣言
class Player;


//プレイヤーのUI処理
class PlayerUI {

public://**パブリック関数**//
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerUI(Player*player);
	~PlayerUI()=default;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// ツリー取得
	/// </summary>
	/// <returns>ツリーデータ</returns>
	GvariTree& GetTree() { return tree_; };

private://**プライベート関数**//

	//プレイヤーのポインタ
	Player* player_;

	//HPゲージ
	std::unique_ptr<Sprite>hpBar_;

	//HPバーの最大サイズ
	float maxBarScale_ = 640;

	//デバッグ用ツリー
	GvariTree tree_ = GvariTree("UI");
};