#pragma once
#include"Vector2.h"
#include"Vector4.h"
#include"Matrix.h"
#include"ParticleData.h"
#include"ParticlePSO/ParticlePSO.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleData/Update/ParticleUpdateCS.h"
#include"ParticleData/Initialize/ParticleInitialize.h"
#include"ParticleData/Emiter/ParticleEmiterCS.h"

#include"GlobalVariable/Tree/GlobalVariableTree.h"
#include<stdint.h>

///パーティクルのマテリアルデータ
struct ParticleMaterialData
{
	//色
	Vector4 color;
	//UVの行列
	Matrix4x4 uvTransform;
	//画像有効フラグ
	int32_t enableTex;
	//棄却量
	float discardNum;
};

//パーティクルマネージャクラス
class ParticleManager {

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="tex"></param>
	void Initialize(int tex);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ処理
	/// </summary>
	/// <param name="name"></param>
	void Debug(const std::string name);

	/// <summary>
	/// エフェクトの出現
	/// </summary>
	/// <param name="pos"></param>
	void SpawnE(const Vector3&pos);

	/// <summary>
	/// エフェクトを出現関数が呼び出されない限り出ないフラグ
	/// </summary>
	/// <param name="onlyImpact">フラグ</param>
	void SetOnlyImpact(bool onlyImpact) { onlyImpact_ = onlyImpact; }

	/// <summary>
	/// エミッターデータ取得
	/// </summary>
	/// <returns></returns>
	EmiterSphere* GetEmiterData() { return emiterData_; }

	/// <summary>
	/// ツリーデータ取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	GlobalVariableTree& GetTreeData(const std::string& name);

private://**プライベート変数**//

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	//DXFのポインタ
	DirectXFunc* DXF_;
	
	//モデルデータ
	ModelData modelData_;

	//パーティクルのパイプライン
	std::unique_ptr<ParticlePSO>pso_;

	//画像のハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE texture_{};

	//パーティクル初期化クラス
	std::unique_ptr<ParticleInitializeCS> particleInitializeCS_;
	//パーティクル更新クラス
	std::unique_ptr<ParticleUpdateCS>particleUpdateCS_;
	//パーティクル発生クラス
	std::unique_ptr<ParticleEmiterCS>emiterCS_;

	//最大エフェクト出現量
	size_t maxDataNum_ = 1024;

#pragma region リソース関係

	//頂点関係
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	//インデックス
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	//パーティクル本体のデータ
	ComPtr<ID3D12Resource> particleResource_;
	Handles SRVHandle_;
	Handles UAVHandle_;

	//データ管理のリスト
	ComPtr<ID3D12Resource> freeListResource_;
	Handles listUAVHandle_;

	//リストの頂点
	ComPtr<ID3D12Resource> freeListIndexResource_;
	Handles counterSRVHandle_;
	Handles counterUAVHandle_;

	//perのリソース
	ComPtr<ID3D12Resource> perResource_;
	PerView* perViewData_ = nullptr;

	//マテリアルのリソース
	ComPtr<ID3D12Resource> materialResource_;
	ParticleMaterialData* materialData_=nullptr;

	//エミッター構造体のリソース
	ComPtr<ID3D12Resource> emiterResource_;
	EmiterSphere* emiterData_ = nullptr;

#pragma endregion

	//インパクトのみ出現フラグ
	bool onlyImpact_ = false;

	//デバッグ用ツリー
	GlobalVariableTree tree_ = GlobalVariableTree("");

};