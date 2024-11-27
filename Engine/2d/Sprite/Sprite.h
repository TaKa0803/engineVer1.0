#pragma once
#include<d3d12.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"
#include"Math/Vector2.h"
#include"WorldTransform/WorldTransform.h"
#include"GvariGroup/GvariGroup.h"

#include<memory>
#include<wrl.h>


struct SpriteMaterial
{
	//色
	Vector4 color;
	//UV座標
	Matrix4x4 uvTransform;
	//画像を使用するか
	int32_t enableTexture;

	float padding[3];
	//棄却量
	float discardNum;
};


class Sprite {
public:

	Sprite();

	~Sprite();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	
	/// <summary>
	/// 画像データ生成
	/// </summary>
	/// <param name="texture">画像番号</param>
	/// <param name="size">元画像のサイズ</param>
	/// <param name="Rect">元画像の描画する範囲</param>
	/// <param name="translate">座標</param>
	/// <param name="rotate">回転量</param>
	/// <param name="scale">描画時の画像サイズ</param>
	/// <param name="anchor">アンカー</param>
	/// <returns>画像サイズ</returns>
	static Sprite* Create(int texture, const Vector2 size,const Vector2 Rect, const Vector2 scale = { 1,1 }, const Vector2 translate = { 640,360 },  const Vector2 anchor = { 0.5f,0.5f }, const float rotate = 0);



	/// <summary>
	/// デバッグウィンドウ表示
	/// </summary>
	/// <param name="name">ウィンドウの名前</param>
	void DrawDebugImGui(const char* name);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="texture">画像の要素番号（無くてもいい）</param>
	void Draw(int texture=-1);

	/// <summary>
/// 描画処理
/// </summary>
/// <param name="texture">画像の要素番号（無くてもいい）</param>
	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE tex);
public:///セッター

	/// <summary>
	/// 座標代入
	/// </summary>
	/// <param name="pos">代入座標</param>
	void SetPosition(const Vector3& pos) { world_.translate_ = pos; }
	
	/// <summary>
	/// 回転量代入
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const float rotate) { world_.rotate_.z = rotate; }

	/// <summary>
	/// サイズ設定
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { world_.scale_ = scale; }

	/// <summary>
	/// サイズ設定（等倍
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const float scale) { world_.scale_ = { scale,scale,1 }; }

	/// <summary>
	/// uvの平行移動代入
	/// </summary>
	/// <param name="uvPos">代入する</param>
	void SetTVTranslate(const Vector2 uvPos) { uvWorld_.translate_.x = uvPos.x; uvWorld_.translate_.y = uvPos.y; }


	/// <summary>
	/// 画像のカラーを設定
	/// </summary>
	/// <param name="color"></param>
	void SetMaterialDataColor(const Vector4& color) { materialData_->color = color; }

	void SetColorAlpha(float alpha) { materialData_->color.w = alpha; }

	/// <summary>
	/// 画像を使うか否か
	/// </summary>
	/// <param name="ans">正否</param>
	void IsEnableTexture(const bool ans) { materialData_->enableTexture = ans; }

	//親子関係取得
	void SetParent(const EulerWorldTransform& parent) { world_.parent_ = &parent; }
public:
	/// <summary>
	/// マテリアルデータ構造体取得
	/// </summary>
	/// <returns>マテリアルデータ</returns>
	SpriteMaterial GetMaterialData() { return *materialData_; }
	
	/// <summary>
	/// スケール取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetScale()const { return world_.scale_; }

	/// <summary>
	/// 座標取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetPosition() { return world_.translate_; }

	/// <summary>
	/// ワールド行列取得
	/// </summary>
	/// <returns></returns>
	const EulerWorldTransform& GetWorld()const { return world_; }

	/// <summary>
	/// ツリー取得
	/// </summary>
	/// <returns></returns>
	GVariTree& GetTree(const std::string&name);


	

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int texture,

	EulerWorldTransform world,

	ID3D12Resource* vertexResource,

	ID3D12Resource* indexResourceSprite,

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite
	);

	
private:



	DirectXFunc* DXF_ = nullptr;

	
	FillMode fillMode_ = FillMode::kSolid;
	BlendMode blendMode_ = BlendMode::kNormal;


	int texture_=-1;

	
	//頂点関係
	ID3D12Resource* vertexResource_=nullptr;
	ID3D12Resource* indexResource_=nullptr;
	//
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	

	//座標関係
	ID3D12Resource* transformationMatrixResource_ = nullptr;;
	//データ
	WorldTransformation* transformationMatrixData_ = nullptr;


	//マテリアル関係
	ID3D12Resource* materialResource_ = nullptr;
	//データ
	SpriteMaterial* materialData_ = nullptr;;
	

	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();

	
	//スプライトのワールドデータ
	EulerWorldTransform world_;

	//uvのWorld
	EulerWorldTransform uvWorld_{};

private:

	//デバッグ用ツリー
	GVariTree tree_=GVariTree("");

};