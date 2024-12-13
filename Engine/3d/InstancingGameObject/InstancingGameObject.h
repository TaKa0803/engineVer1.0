#pragma once

#include"InstancingModelManager/InstancingModelManager.h"
#include"WorldTransform/WorldTransform.h"
#include<vector>

//インスタンシングのゲームオブジェクト
class InstancingGameObject {

public:	//**パブリック変数**//

	//モデルのタグ
	std::string tag_;

	//ワールド
	EulerWorldTransform world_{};

	//インスタンシングモデルマネージャー
	InstancingModelManager* IMM_=nullptr;

	//色
	Vector4 color_ = { 1,1,1,1 };

	//デバッグツリー
	GlobalVariableTree tree_ = GlobalVariableTree("");

public:	//**パブリック関数**//

	//開放処理
	virtual ~InstancingGameObject();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="tag">モデルタグ</param>
	virtual void Initialize(const std::string& tag);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// デバッグ表示
	/// </summary>
	/// <param name="name">名前</param>
	virtual void Debug(const char* name);

public:	//**セッター**//

	/// <summary>
	/// スケール等倍
	/// </summary>
	/// <param name="scale">等倍サイズ</param>
	void SetScale(float scale) { world_.scale_ = { scale,scale ,scale }; }

	/// <summary>
	/// 親設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const EulerWorldTransform* parent) { world_.parent_ = parent; }

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color) { color_ = color; }

public:	//**ゲッター**//

	/// <summary>
	/// デバッグツリーを取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	GlobalVariableTree& GetDebugTree(const std::string&name = "オブジェクト");

};

