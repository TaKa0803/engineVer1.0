#pragma once
#include"Sprite/Sprite.h"

//ボスの前方宣言
class Boss;

//ボス関係のUI処理
class BossUI {
public: //**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="boss">ボスのポインタ</param>
	BossUI(Boss* boss);
	~BossUI()=default;

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

public: //**ゲッター**//

	/// <summary>
	///	ツリーデータ取得
	/// </summary>
	/// <returns>ツリーデータ</returns>
	GlobalVariableTree& GetTree() { return tree_; }

private: //**プライベート変数**//

	//ボスのポインタ
	Boss* boss_=nullptr;

	//HPバー画像
	std::unique_ptr<Sprite>hpBar_=nullptr;

	//最大サイズ
	float maxScale_=0;

private: //**デバッグ用変数**//

	//デバッグツリー
	GlobalVariableTree tree_= GlobalVariableTree("");


};