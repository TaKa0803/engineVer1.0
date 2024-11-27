#pragma once
#include"Sprite/Sprite.h"

//前方宣言
class Boss;

class BossUI {
public:

	//コンストラクタ
	BossUI(Boss* boss);
	~BossUI()=default;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public:

	//ツリーデータ取得
	GVariTree& GetTree() { return tree_; }

private:

	//HPバー
	std::unique_ptr<Sprite>hpBar_=nullptr;
	Vector4 barColor_ = { 1,1,1,1 };


private:
	//デバッグツリー
	GVariTree tree_= GVariTree("");


};