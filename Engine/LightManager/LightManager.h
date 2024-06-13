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

class LightManager {

public:
	static LightManager* GetInstance();

private:
	LightManager() = default;
	~LightManager() = default;
	LightManager(const LightManager& o) = delete;
	const LightManager& operator=(const LightManager& o) = delete;

public:

	void Initialize();

	void PreUpdate();

	void SetPLight(const PointLight& data) { pLights_ = data; }

	void SetDLight(const DirectionalLight& data) { dLights_ = data; }

	PointLight GetPLight() { return pLights_; }

	DirectionalLight GetDLight() { return dLights_; }
private:

	PointLight pLights_ ;

	DirectionalLight dLights_;
};