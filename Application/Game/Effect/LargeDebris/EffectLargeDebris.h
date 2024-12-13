#pragma once
#include"3d/InstancingGameObject/InstancingGameObject.h"

#include<list>

//大きながれきのエフェクトクラス
class  EffectLargeDebris : public InstancingGameObject {

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	EffectLargeDebris();

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

	/// <summary>
	/// エフェクト出現処理
	/// </summary>
	/// <param name="pos">出現座標</param>
	void Spawn(const Vector3& pos);

public:	//**パブリック変数**//

	//エフェクトのパラメータ
	struct  Parameter {

		//ワールド座標
		EulerWorldTransform world{};

		//速度
		Vector3 velo{};

		//加速度
		Vector3 acce{};

		//生存カウント
		float liveCount = 0;

		//色
		Vector4 color{};

		//生存フラグ
		bool isDead = false;
	};

	//エフェクトデータ群
	std::list<Parameter>effects_;

public:	//**パラメータ**//

	//出現範囲
	Vector2 spawnWide_ = { 5.0f, 15.0f};

	//初速度
	Vector2 spd_ = { 10.0f,40.0f };

	//重力
	float gravity_ = 10.0f;

	//同時生成数
	Vector2 spawnSameTimeNum_ = {5,10};

	//生存時間
	float liveSec_ = 3.0f;

	//エフェクトの色1
	Vector4 color1_{};

	//エフェクトの色2
	Vector4 color2_{};

	//エフェクトの色3
	Vector4 color3_{};
};