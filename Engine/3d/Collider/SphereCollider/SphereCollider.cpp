#include "SphereCollider.h"
#include"InstancingModelManager/InstancingModelManager.h"
#include"ColliderOBB/OBBCollider.h"
#include"IsCollisions.h"
#include<imgui.h>


bool SphereCollider::isDraw_ = true;

SphereCollider::SphereCollider() {

}


void SphereCollider::Initialize(const std::string& tag, const EulerWorldTransform& world) {
	//タグ整合性チェック
	InstancingGameObject::Initialize("sphere");

	//親を設定
	world_.parent_ = &world;

	//名前設定
	colliderTag_ = tag;

	//モデル設定
	IMM_->SetFillMode(tag_, FillMode::kWireFrame);
	IMM_->SetAlpha(tag_, alpha_);
	IMM_->SetEnableTexture(tag_, false);
	IMM_->SetEnableShader(tag_, false);
}

void SphereCollider::Initialize(const std::string& tag)
{
	//タグ整合性チェック
	InstancingGameObject::Initialize("sphere");

	//コライダー名設定
	colliderTag_ = tag;

	//モデル設定
	IMM_->SetFillMode(tag_, FillMode::kWireFrame);
	IMM_->SetAlpha(tag_, alpha_);
	IMM_->SetEnableTexture(tag_, false);
	IMM_->SetEnableShader(tag_, false);
}


void SphereCollider::Update() {

	//現在のワールド情報を保存
	preWorld_ = world_;
	
	//サイズを設定
	world_.scale_ = { scale_,scale_,scale_ };

	//更新
	world_.UpdateMatrix();
	
}

void SphereCollider::Draw() {
	//デバッグのみ描画
#ifdef _DEBUG
	//描画フラグが有効の場合のみ描画
	if (isDraw_) {
		InstancingModelManager::GetInstance()->SetData(tag_, world_, color_);
	}
#endif // _DEBUG

}

bool SphereCollider::IsCollision(const SphereCollider* sphere, Vector3& backVec) {
	//各点取得
	Vector3 pos = sphere->world_.GetWorldTranslate();
	Vector3 myP = world_.GetWorldTranslate();

	//各半径取得
	float wide = GetAllScaleX(sphere->GetWorld());
	float myW = GetAllScaleX(world_);

	//距離計算
	Vector3 sub = myP - pos;

	//計算した距離を保存
	Vector3 leng = sub;
	//ヒットチェック
	if (Length(sub) < wide + myW) {

		//正規化
		sub.SetNormalize();

		//基コライダーから自分への向きベクトル作成
		sub *= (wide + myW);

		//押し出し量求める
		backVec = sub - leng;

		if (!isActive_ || !sphere->isActive_) {
			return false;
		}
		//色を変更
		SetColor(true);

		//返却
		return true;
	}
	//色を変更
	SetColor(false);
	//当たっていない
	return false;
}



bool SphereCollider::IsCollision(OBBCollider& obb, Vector3& backVec, float divisionVolume)
{

#pragma region OBBのワールド行列をスケールなしで作成

	//OBBのworld行列生成
	Matrix4x4 OBBM = obb.GetWorldM();

	//逆行列
	Matrix4x4 inverseM = obb.GetInverseWorldM();

	//すべてのスケールからサイズ取得
	Vector3 size = GetAllScale(obb.world_);

#pragma endregion

	float radius = GetAllScaleX(world_);

	float t = 0;
	while (t <= 1.0f) {

		//過去位置から現在位置までの場所取得
		Vector3 pos = Lerp(preWorld_.GetWorldTranslate(), world_.GetWorldTranslate(), t);

		//スフィアコライダーの座標をOBBのローカル空間に出る
		Vector3 sphereLocal = Transform(pos, inverseM);

		//AABB取得
		AABB aabb_ = { .minV = -size,.maxV = size };
		//Sphere取得
		Sphere sphere = { sphereLocal,GetAllScaleX(world_) };

		//当たり判定
		Vector3 saikin{};
		if (InCollision(aabb_, sphere, saikin)) {


			//OBBLocalPosCange
			saikin = Transform(saikin, OBBM);

			//mosionajiiti
			if (world_.GetWorldTranslate() == saikin) {
				//スフィアコライダーの座標をOBBのローカル空間に出る
				sphereLocal = Transform(preWorld_.GetWorldTranslate(), inverseM);
				//Sphere取得
				sphere = { sphereLocal,GetAllScaleX(world_)};
				InCollision(aabb_, sphere, saikin);

				saikin = Transform(saikin, OBBM);

				Vector3 velo = preWorld_.GetWorldTranslate() - saikin;
				velo.SetNormalize();
				velo *= radius;

				backVec = velo;

			}
			else {
				///押し出しベクトルを利用して計算
				//最近接点から円の中心点への向きベクトルを算出
				Vector3 velo = world_.GetWorldTranslate() - saikin;
				//正規化

				Vector3 norVe = velo;
				norVe.SetNormalize();
				//半径分伸ばす
				norVe *= radius;

				//渡す
				backVec = norVe - velo;


			}

#ifdef _DEBUG
			////最近接点描画
			EulerWorldTransform sWo;
			sWo.translate_ = saikin;
			sWo.scale_ = { 0.1f,0.1f,0.1f };
			sWo.UpdateMatrix();
			IMM_->SetData("sphere", sWo);
#endif // _DEBUG
			obb.SetColor(true);
			//色の変更
			SetColor(true);

			//貫通しているかの処理
			//最近接点から過去
			Vector3 v1 = preWorld_.GetWorldTranslate() - saikin;
			//最近接点から現在
			Vector3 v2 = world_.GetWorldTranslate() - saikin;
			v2 *= -1;
			//真反対の時押し出し量変更
			if (v1 == v2) {
				//現在から過去へのベクトル取得
				Vector3 npVelo = preWorld_.GetWorldTranslate() - world_.GetWorldTranslate();
				npVelo.SetNormalize();
				//現在位置から最近接点までの向きベクトル取得
				v1 = preWorld_.GetWorldTranslate() - saikin;
				//求めた長さを押し出し量にタス

				backVec += v1;
			}

			return true;
		}
		else {

#ifdef _DEBUG
			//最近接点描画
			EulerWorldTransform sWo;
			sWo.translate_ = Transform(saikin, OBBM);
			sWo.scale_ = { 0.1f,0.1f,0.1f };
			sWo.UpdateMatrix();
			IMM_->SetData("sphere", sWo);
#endif // _DEBUG
		}


		t += 1.0f / divisionVolume;
	}


	//色の変更
	SetColor(false);
	obb.SetColor(false);

	return false;


}



void SphereCollider::SetColor(bool hit)
{
	if (hit) {
		color_ = hitColor;
	}
	else {
		color_ = normalColor;
	}
}

void SphereCollider::UpdateMatrix()
{
	world_.UpdateMatrix();
}

GlobalVariableTree& SphereCollider::GetDebugTree(const std::string& name)
{

	tree_.name_ = name;

	tree_.SetTreeData(world_.GetDebugTree());
	tree_.SetValue("有効", &isActive_);
	tree_.SetValue("描画", &isDraw_);
	tree_.SetValue("通常色", &normalColor);
	tree_.SetValue("ヒット色", &hitColor);
	tree_.SetValue("サイズ", &scale_);

	return tree_;
}



float SphereCollider::GetRadius()
{
	return GetAllScaleX(world_);
}




