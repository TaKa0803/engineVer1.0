#pragma once
#include"InstancingModelManager/InstancingModelManager.h"
#include"GvariGroup/GvariGroup.h"

#include<list>
class EffectNormal {

public:

	struct EffectData {
		EulerWorldTransform world;
		Vector3 velo;
		Vector3 acce;

		Vector4 color={1,1,1,1};

		float count;
		float currentCount = 0;


		bool isDead = false;
	};

public:
	EffectNormal(const std::string& tag,const std::string& name);
	~EffectNormal()=default;

	void Initialize();

	void Update();

	void Draw();

	void SpawnEffect(const Vector3&pos);

private:
	InstancingModelManager* IMM_ = nullptr;
private:

	//エフェクト群
	std::list<EffectData>datas_;

	struct EmitData {

		//生成最大範囲
		Vector3 spawnMaxWide;
		//生成最小範囲
		Vector3 spawmMinWide;

		//最小最大サイズ
		Vector2 size{ 0.1f,0.5f };

		//最小最大生成量
		Vector2 numSpawn{ 10,20 };

		//最大ベクトル方向
		Vector3 veloMax;
		//最小ベクトル方向
		Vector3 veloMin;

		//加速度
		Vector3 acce{0,0,0};

		//最小、最大初速度
		Vector2 spd{10.0f,20.0f};

		//最小最大生存時間
		Vector2 deadCount{0.1f,0.5f};

	}emitData_{};

private:

	std::string tag_;

	
};