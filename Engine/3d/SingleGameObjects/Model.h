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
#include"Camera/Camera.h"
#include"InstancingGameObject/InstancingModel.h"

void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

class Model {
public:


	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 円の作成
	/// </summary>
	/// <param name="kSubdivision">分割量</param>
	/// <param name="enableLighting">影をつけるか</param>
	/// <returns>モデルデータ</returns>
	static Model* CreateSphere(float kSubdivision, bool enableLighting, const std::string& filePath = "");

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
	/// <param name="viewProjection"></param>
	/// <param name="texture"></param>
	void Draw(const Matrix4x4& WVP, const Camera& camera, Vector3 pointlight = { 0,0,0 }, int texture = -1);

	void PlayAnimation(int animeNum = 0);


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

	/// <summary>
	/// uv座標の変換
	/// </summary>
	/// <param name="uv">uvMatrix</param>
	void SetUV(Matrix4x4 uv) { materialData_->uvTransform = uv; }

	void SetUVTranslate(Vector2 pos) { uvpos.x = pos.x; uvpos.y = pos.y; }

	void SetUVScale(Vector3 scale) { uvscale = scale; }

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

	const Vector3 GetUVScale()const { return uvscale; }
private:


	//初期化
	void Initialize(
		ModelAllData data,
		std::string name,
		int point,
		ID3D12Resource* vertexRtea,
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		ID3D12Resource* wvpResource,
		WorldTransformation* wvpData
	);

private:

	DirectXFunc* DXF_;

	ModelManager* modelM_;

	ModelAllData modelData_;

	Skeleton skeleton_;

	SkinCluster skinCluster_;

	//ジョイントの描画データ
	InstancingModelManager* jointM__;
	std::string jointMtag_;
	bool drawJoint_ = false;
	bool drawModel_ = true;
	//埋めるか否か
	FillMode fillMode_ = FillMode::kSolid;
	BlendMode  blendMode_ = BlendMode::kNormal;

	std::string name;

	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	bool isDebug = false;

	//頂点数
	int point_;

	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};


	ID3D12Resource* vertexData_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	ID3D12Resource* wvpResource_;
	WorldTransformation* wvpData_ = nullptr;

	//マテリアル
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

	Vector3 uvpos{};
	Vector3 uvscale{ 1.0f,1.0f,1.0f };
	Vector3 uvrotate{};

	float animationTime = 0.0f;
	Matrix4x4 localM_;
};




