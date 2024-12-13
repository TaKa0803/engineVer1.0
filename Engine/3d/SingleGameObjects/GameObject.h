#pragma once

#include"SingleGameObjects/Model.h"
#include"WorldTransform/WorldTransform.h"
#include<vector>

//ゲームオブジェクト
class GameObject {

public:	//**パブリック変数**//

	//モデルデータ
	Model* model_ = nullptr;

	//座標
	EulerWorldTransform world_;
	
public:	///**パブリック関数**//

	/// <summary>
	/// 解放処理
	/// </summary>
	virtual ~GameObject();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filepath">モデルまでのパス</param>
	virtual void Initialize(const std::string& filepath);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="tex">指定画像</param>
	virtual void Draw( int tex = -1);
};