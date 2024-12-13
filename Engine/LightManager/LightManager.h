#pragma once
#include"Vector4.h"
#include"Vector3.h"

#include<list>

struct DirectionalLight {
	Vector4 color;
	Vector3 direction = {0,-1,0};
	float intensity =1;
	//float padding2[3];
};

//ポイントライト
struct PointLight
{
	Vector4 color;//ライトの色
	Vector3 position;//座標
	float intensity = 1;//光度
	float radius = 1;
	float decay = 1;
	//float padding[2];
};

//明かりのマネージャ
class LightManager {

public://**シングルトンパターン**//
	static LightManager* GetInstance();
private:
	LightManager() = default;
	~LightManager() = default;
	LightManager(const LightManager& o) = delete;
	const LightManager& operator=(const LightManager& o) = delete;

public://**パブリック関数**//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新前処理
	/// </summary>
	void PreUpdate();

	/// <summary>
	/// ポイントライトのセット
	/// </summary>
	/// <param name="data">送るデータ</param>
	void SetPLight(const PointLight& data) { pLights_ = data; }

	/// <summary>
	/// ディレクショナルライトのセット
	/// </summary>
	/// <param name="data">送るデータ</param>
	void SetDLight(const DirectionalLight& data) { dLights_ = data; }

	/// <summary>
	/// Pライト取得
	/// </summary>
	/// <returns></returns>
	PointLight GetPLight() { return pLights_; }

	/// <summary>
	/// Dライト取得
	/// </summary>
	/// <returns></returns>
	DirectionalLight GetDLight() { return dLights_; }

private://**プライベート変数**//

	//Pライトデータ
	PointLight pLights_ ;

	//Dライトデータ
	DirectionalLight dLights_;
};