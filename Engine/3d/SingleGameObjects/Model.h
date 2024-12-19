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
#include"ComputeShaders/SkinningCS.h"
#include"GlobalVariable/Tree/GlobalVariableTree.h"

//モデル倉さう
class Model {
public://**プライベート関数**//

	Model();
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

public://**パブリック関数**//

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="WVP"></param>
	/// <param name="texture"></param>
	void Draw(const Matrix4x4& WVP, int texture = -1);

	/// <summary>
	/// アニメーション変更
	/// </summary>
	/// <param name="animeNum">番号</param>
	/// <param name="count">変わりきるまでの速度</param>
	void ChangeAnimation(const std::string&animeName, float sec);

public://**セッター**//

	/// <summary>
	/// アニメーションループフラグ
	/// </summary>
	/// <param name="isLoop">フラグ</param>
	void SetAnimationRoop(bool isLoop) { isAnimeRoop_ = isLoop; };

	/// <summary>
	/// アニメーション有効化フラグ
	/// </summary>
	/// <param name="isActive">フラグ</param>
	void SetAnimationActive(bool isActive) { isAnimationActive_ = isActive; }

	/// <summary>
	/// Gvariツリーの作成
	/// </summary>
	/// <param name="treeName">各パラメータの含まれたTreeの返却</param>
	GlobalVariableTree& SetDebugParam(const std::string &treeName = "モデル");

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

	/// <summary>
	/// アニメーションタイムを引数側から指定する
	/// </summary>
	/// <param name="isAct"></param>
	/// <param name="t"></param>
	void SetAnimationTime(bool isAct, float t) { isSetATime_ = isAct; setAt_ = t; };

	/// <summary>
	/// 色の取得
	/// </summary>
	/// <returns>色</returns>
	const Vector4 GetColor() { return materialData_->color; }

	const Material* GetMaterialData() { return materialData_; }

	//ジョイント取得
	Matrix4x4 GetJoint(const std::string& name);

private://**プライベート関数**//

	//初期化
	void Initialize(
		ModelAllData data,
		std::string name,
		int point
	);

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// アニメーション処理
	/// </summary>
	/// <param name="skeleton">スケルトン</param>
	/// <param name="animation">animationデータ</param>
	/// <param name="animationTime">アニメーション時間</param>
	/// <param name="designation"フラグ></param>
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime, bool designation);

public://**パブリック変数**//

	//UVのworldデータ
	EulerWorldTransform uvWorld_{};

	//埋めるか否か
	FillMode fillMode_ = FillMode::kSolid;
	//各ブレンドモード
	BlendMode  blendMode_ = BlendMode::kNormal;

	//マテリアルデータ
	Material* materialData_ = nullptr;

	//アニメーションフラグ
	bool isAnimationActive_ = false;
	//animationの一周までの秒数
	float animationRoopSecond_ = 1.0f;

private://**プライベート関数**//

	//DXFのポインタ
	DirectXFunc* DXF_;

	//モデルマネージャ
	ModelManager* modelM_;

	//スキニング処理
	std::unique_ptr<SkinningCS>skinningCS_;

	//モデルとanimationデータ
	ModelAllData modelData_;

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
	ID3D12Resource* vertexResource_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	VertexData* vertexData_ = nullptr;
#pragma region 各追加送信データ
	//ワールドデータリソース
	ID3D12Resource* wvpResource_;
	WorldTransformation* wvpData_ = nullptr;

	//マテリアルリソース
	ID3D12Resource* materialResource_;
	
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

	//現在のアニメ番号
	//int animeNumber_ = 0;
	
	//0-1でアニメーションを動かすか否か
	bool isSetATime_ = false;
	//0-1の値
	float setAt_ = 0;

	//アニメーションカウント
	float animationTime_ = 0.0f;


	//アニメーションをループするか
	bool isAnimeRoop_ = true;


	//現在のアニメーション名
	std::string nowAnimeName_="";

	//ボーンのないanimationキューブ用のlocalMatrix
	Matrix4x4 localM_;


	//補完フラグ
	bool isSupplementation_ =false;

	//カウント変数
	float supplementationCount_ = 0;
	//補完カウント
	float maxSupplementationCount_;
	//過去アニメーション
	std::vector<QuaterinionWorldTransform> savedT;
#pragma endregion


	//モデルタイプ
	ModelDataType modelType_;

private://**デバッグ用**//

	GlobalVariableTree tree = GlobalVariableTree("model");
};




