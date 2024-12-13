#pragma once
#include"InstancingModelManager/InstancingModelManager.h"
#include"GvariGroup/GvariGroup.h"

#include<list>

//簡単なエフェクトを容易に作成できるクラス
class EffectNormal {

public:	//**パブリック変数**//

	//エフェクトのデータ
	struct EffectData {
		EulerWorldTransform world;	//ワールド座標
		Vector3 velo;				//速度
		Vector3 acce;				//加速度

		Vector4 color={1,1,1,1};	//色

		float count;				//生存時間
		float currentCount = 0;		//時間カウント

		bool isDead = false;		//死亡フラグ
	};

public:	//**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="tag">モデルタグ</param>
	/// <param name="name">デバッグツリーの名前</param>
	EffectNormal(const std::string& tag,const std::string& name);
	~EffectNormal()=default;

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
	/// <param name="pos"></param>
	void SpawnEffect(const Vector3&pos);

private: //**参照物**//

	//インスタンシングモデルマネージャ
	InstancingModelManager* IMM_ = nullptr;

private: //**プライベート変数**//

	//エフェクト群
	std::list<EffectData>datas_;

	//発生データ
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

	//モデルタグ
	std::string tag_;

	
};