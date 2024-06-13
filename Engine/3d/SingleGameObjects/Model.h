#pragma once
#include<d3d12.h>
#include<wrl.h>
#include<string>
#include"DirectXFunc/DirectXFunc.h"
#include"SingleGameObjects/ObjectPSO.h"
#include"Math/Vector2.h"
#include"Math/Matrix.h"
#include"struct.h"

#include"SingleModelManager/ModelManager.h"
#include"InstancingModelManager/InstancingModelManager.h"
#include"LightManager/LightManager.h"

void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

class Model {
public:


	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;


	/// <summary>
	/// OBJ作成
	/// </summary>
	/// <param name="filePath">resources以降のフォルダ</param>
	/// <returns><モデルデータ/returns>
	static Model* CreateFromOBJ(const std::string& filePath);


public:

	void UpdateAnimation();


	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="WVP"></param>
	/// <param name="texture"></param>
	void Draw(const Matrix4x4& WVP, int texture = -1);


	/// <summary>
	/// Debug用ImGui表示
	/// </summary>
	/// <param name="name"></param>
	void DebugParameter(const char* name);

	/// <summary>
	/// シェーダー処理の切り替え
	/// </summary>
	/// <param name="ans">影をつけるか</param>
	void IsEnableShader(bool ans) { materialData_->enableLighting = ans; }

	/// <summary>
	/// 画像を使用するか
	/// </summary>
	/// <param name="ans">画像を使うか</param>
	void IsEnableTexture(bool ans) { materialData_->enableTexture = ans; }
#pragma region セッター
	//uv座標設定
	void SetUVTranslate(Vector2 pos) { uvWorld_.translate_.x = pos.x; uvWorld_.translate_.y = pos.y; }
	//uvサイズ設定
	void SetUVScale(Vector3 scale) { uvWorld_.scale_ = scale; }

	/// <summary>
	/// 色の変更
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(Vector4 color) { materialData_->color = color; }

	void SetAlpha(float alpha) { materialData_->color.w = alpha; }

	void SetBlendMode(BlendMode blend) { blendMode_=blend; }

	void SetFillMode(FillMode fillmode) { fillMode_=fillmode; }

#pragma endregion

	/// <summary>
	/// 色の取得
	/// </summary>
	/// <returns>色</returns>
	const Vector4 GetColor() { return materialData_->color; }

	const Material* GetMaterialData() { return materialData_; }

private:

	//初期化
	void Initialize(
		ModelAllData data,
		std::string name,
		int point,
		ID3D12Resource* vertexRtea,
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView
	);

public:

	//UVのworldデータ
	EulerWorldTransform uvWorld_{};

	//埋めるか否か
	FillMode fillMode_ = FillMode::kSolid;
	//各ブレンドモード
	BlendMode  blendMode_ = BlendMode::kNormal;


private:

	DirectXFunc* DXF_;

	ModelManager* modelM_;

	//モデルとanimationデータ
	ModelAllData modelData_;

	//スケルトンデータ
	Skeleton skeleton_;
	//スキンanimationデータ
	SkinCluster skinCluster_;

	
	//ジョイントの描画データ
	InstancingModelManager* jointM__;
	std::string jointMtag_;
	bool drawJoint_ = false;
	bool drawModel_ = true;

	//画像handle
	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	//頂点数
	int point_;

	//インデックスリソース
	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	//vertexリソース
	ID3D12Resource* vertexData_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

#pragma region 各追加送信データ
	//ワールドデータリソース
	ID3D12Resource* wvpResource_;
	WorldTransformation* wvpData_ = nullptr;

	//マテリアルリソース
	ID3D12Resource* materialResource_;
	Material* materialData_ = nullptr;

	//ディレクションライト
	ID3D12Resource* directionalLightResource_;
	DirectionalLight* directionalLightData_ = nullptr;

	//カメラ
	ID3D12Resource* cameraResource_;
	Camera4GPU* cameraData_ = nullptr;

	//ポイントライト
	ID3D12Resource* pointlightResource_;
	PointLight* pointLightData_;
#pragma endregion



#pragma region アニメーション関係
	//アニメーションフラグ
	bool isAnimationActive_ = false;
	float animationTime = 0.0f;
	//animationの一周までの秒数
	float animationRoopSecond_ = 1.0f;

	//アニメーション要素番号
	int animeNum_ = 0;

	//ボーンのないanimationキューブ用のlocalMatrix
	Matrix4x4 localM_;
#pragma endregion


	//モデルタイプ
	ModelDataType modelType_;
};




