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

#include"GvariGroup/GvariGroup.h"
#include<stdint.h>

struct ParticleMaterialData
{
	Vector4 color;
	Matrix4x4 uvTransform;
	int32_t enableTex;
	float discardNum;
};

class ParticleManager {

public:
	ParticleManager();
	~ParticleManager();

	//初期化処理
	void Initialize(int tex);

	//データ更新処理
	void Update();

	//描画
	void Draw();

	//デバッグ
	void Debug(const std::string name);

	void SpawnE(const Vector3&pos);

	void SetOnlyImpact(bool onlyImpact) { onlyImpact_ = onlyImpact; }

	EmiterSphere* GetEmiterData() { return emiterData_; }

	GlobalVariableTree& GetTreeData(const std::string& name);
private:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	DirectXFunc* DXF_;
	
	ModelData modelData_;

	std::unique_ptr<ParticlePSO>pso_;

	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	std::unique_ptr<ParticleInitializeCS> particleInitializeCS_;
	std::unique_ptr<ParticleUpdateCS>particleUpdateCS_;
	std::unique_ptr<ParticleEmiterCS>emiterCS_;

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


	ComPtr<ID3D12Resource> perResource_;
	PerView* perViewData_ = nullptr;

	ComPtr<ID3D12Resource> materialResource_;
	ParticleMaterialData* materialData_=nullptr;

	ComPtr<ID3D12Resource> emiterResource_;
	EmiterSphere* emiterData_ = nullptr;

#pragma endregion

	//
	bool onlyImpact_ = false;

	GlobalVariableTree tree_ = GlobalVariableTree("");

};