#pragma once
#include "SingleGameObjects/GameObject.h"
#include"Camera/Camera.h"

//地面
class Plane :public GameObject {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Plane();
	~Plane();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Draw();


};

