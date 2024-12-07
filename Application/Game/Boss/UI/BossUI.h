#pragma once
#include"Sprite/Sprite.h"

//前方宣言
class Boss;

class BossUI {
public:

	//コンストラクタ
	BossUI(Boss* boss);
	~BossUI()=default;

	void Initialize();

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

	Boss* boss_=nullptr;

	//HPバー
	std::unique_ptr<Sprite>hpBar_=nullptr;

	//最大サイズ
	float maxScale_=0;
private:
	//デバッグツリー
	GVariTree tree_= GVariTree("");


};