#pragma once
#include"InstancingGameObject/InstancingGameObject.h"

//OBBコライダー前方宣言
class OBBCollider;

//円コライダー
class SphereCollider : InstancingGameObject {

public://パブリック変数

	//有効処理
	bool isActive_ = true;

public://**パブック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SphereCollider();
	~SphereCollider()=default;

	/// <summary>
	/// 親を設定して初期化
	/// </summary>
	/// <param name="tag">コライダー名</param>
	/// <param name="parent">親</param>
	void Initialize(const std::string& tag,const EulerWorldTransform& parent);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="tag">コライダー名</param>
	void Initialize(const std::string& tag);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグのみ描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 当たり判定チェック
	/// </summary>
	/// <param name="sphere">元となるコライダー</param>
	/// <param name="backVec">押し戻しベクトル</param>
	/// <returns>押し戻しベクトルに値を渡し成否チェック</returns>
	bool IsCollision(const SphereCollider* sphere, Vector3& backVec);

	/// <summary>
	/// 当たり判定チェック
	/// </summary>
	/// <param name="obb">元となるコライダー</param>
	/// <param name="backVec">押し出しベクトル</param>
	/// <returns>引数の押し出しベクトルに値を渡して成否チェック</returns>
	bool IsCollision(OBBCollider& obb, Vector3& backVec, float divisionVolume);

public://**セッター**//

	//透明度設定
	void SetAlpha(const float alpha) { alpha_ = alpha; }

	//半径設定
	void SetRadius(float wide) {world_.scale_ = { wide,wide,wide };}

	//座標設定
	void SetTranslate(const Vector3& pos) { world_.translate_ = pos; world_.UpdateMatrix(); }

	//フラグに併せて色変更
	void SetColor(bool hit);

	//当たった時に行列更新するといい
	void UpdateMatrix();

public://**ゲッター**//

	//ツリー取得
	GlobalVariableTree& GetDebugTree(const std::string& name = "円コライダー");

	//半径取得
	float GetRadius();

	//ローカル平行移動取得
	//Vector3 GetTranslate() { return world_.translate_; }

	//コライダータグ取得
	std::string GetTag()const { return colliderTag_; }

	//ワールド取得
	EulerWorldTransform GetWorld()const { return world_; }

	//過去ワールド取得
	EulerWorldTransform GetPreWorld()const { return preWorld_; }

private://**プライベート変数**//

	//ターゲット
	const EulerWorldTransform* parent_;

	//コライダータグ
	std::string colliderTag_;

	//描画するか否か(すべて
	static bool isDraw_;

	//過去の状態
	EulerWorldTransform preWorld_;

	//透明度
	float alpha_ = 0.5f;

	//通常色
	Vector4 normalColor = { 1,1,1,alpha_ };

	//ヒット時色
	Vector4 hitColor = { 1,0,0,alpha_ };

	//円サイズ
	float scale_ = 1.0f;

	//デバッグ用ツリー
	GlobalVariableTree tree_=GlobalVariableTree("");



};