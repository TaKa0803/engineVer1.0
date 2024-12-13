#pragma once
#include"Sprite/Sprite.h"

//ゲームに使われるUIのクラス
class GameUI {

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameUI();
	~GameUI()=default;

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

private: //**プライベート変数

	//ボタンアイコン
	//B
	std::unique_ptr<Sprite>BButton_;
	//A
	std::unique_ptr<Sprite>AButton_;
	//X
	std::unique_ptr<Sprite>XButton_;

	//攻撃ボタンアイコン
	//パンチ
	std::unique_ptr<Sprite>punchSp_;
	//ダッシュ
	std::unique_ptr<Sprite>dashSp_;
	//回避
	std::unique_ptr<Sprite>rollSp_;

};