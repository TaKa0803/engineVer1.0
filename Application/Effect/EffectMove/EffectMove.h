#pragma once
#include"ParticleManager.h"

//移動時のエフェクトクラス
class EffectMove {

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	EffectMove();
	~EffectMove()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="color">エフェクトの色</param>
	void Initialize(const Vector4& color);

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
	/// <param name="pos"></param>
	void SpawnE(const Vector3& pos);

private: //**プライベート変数**//

	//パーティクルクラス
	std::unique_ptr<ParticleManager>particleM_;
};