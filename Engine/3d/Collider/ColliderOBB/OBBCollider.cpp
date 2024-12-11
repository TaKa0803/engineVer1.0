#include "OBBCollider.h"
#include<imgui.h>
#include"SphereCollider/SphereCollider.h"


bool OBBCollider::isDraw_ = true;

void OBBCollider::Initialize(const std::string& tag, const EulerWorldTransform& parent)
{
	//インスタンシングの初期化
	InstancingGameObject::Initialize("box");
	//親子関係設定
	world_.parent_ = (&parent);
	//コライダーのタグ設定
	colliderTag_ = tag;

	//画像を切る
	IMM_->SetEnableTexture(tag_, false);
	//ワイヤーフレーム表示
	//IMM_->SetFillMode(tag_, FillMode::kWireFrame);
	//影の削除
	IMM_->SetEnableShader(tag_, false);
	//透明度設定
	IMM_->SetAlpha(tag_, alpha_);
}

void OBBCollider::Initialize(const std::string& tag)
{
	//院スタン寝具の初期化
	InstancingGameObject::Initialize("box");

	//コライダーのタグ設定
	colliderTag_ = tag;

	//画像を切る
	IMM_->SetEnableTexture(tag_, false);
	//ワイヤーフレーム表示
	IMM_->SetFillMode(tag_, FillMode::kWireFrame);
	//影の削除
	IMM_->SetEnableShader(tag_, false);
	//透明度設定
	IMM_->SetAlpha(tag_, alpha_);
}

void OBBCollider::Update()
{
	//過去の位置を保存
	preWorld_ = world_;

	//ワールドデータを更新
	world_.UpdateMatrix();

}

void OBBCollider::Draw()
{
#ifdef _DEBUG
	if (isDraw_) {
		InstancingModelManager::GetInstance()->SetData(tag_, world_, color_);
	}
#endif // _DEBUG
}

void OBBCollider::Debug(const char* name)
{

#ifdef _DEBUG
	ImGui::Begin(name);
	ImGui::DragFloat3("trans", &world_.translate_.x, 0.10f);
	ImGui::DragFloat3("rotation", &world_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &world_.scale_.x, 0.01f);

	ImGui::ColorEdit4("hit color", &hitColor.x);
	ImGui::Checkbox("isDraw", &isDraw_);
	ImGui::Text("dot : %f", getDot);
	ImGui::End();
#endif // _DEBUG


}

bool OBBCollider::IsCollision(SphereCollider* collider, Vector3& backVec, int division)
{
	if (division < 1) {
		division = 1;
	}
#pragma region OBBのワールド行列をスケールなしで作成
	//回転量取得
	Vector3 rotate = GetAllRotate(world_);

	//回転行列
	Matrix4x4 rotateM = MakeRotateXMatrix(rotate.x) * (MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z));
	//座標行列
	Matrix4x4 translateM = MakeTranslateMatrix(world_.GetWorldTranslate());
	//スケールは使わない（sizeで使う
	Matrix4x4 scaleM = MakeScaleMatrix({ 1,1,1 });
	//OBBのworld行列生成
	OBBM_ = scaleM * (rotateM * translateM);

	//逆行列
	inverseM_ = Inverse(OBBM_);

	//すべてのスケールからサイズ取得
	Vector3 size = GetAllScale(world_);

	//AABB取得
	aabb_ = { .minV = -size,.maxV = size };
#pragma endregion

	float addNum =  1.0f/division;
	float t = 0;

	//ワールド座標
	Vector3 preCWorld = collider->GetPreWorld().GetWorldTranslate();
	//ワールド座標
	Vector3 CWorld = collider->GetWorld().GetWorldTranslate();


	while (true) {

		//過去地点から現在地点まで段階的に探査する
		Vector3 spehrePos = Lerp(preCWorld, CWorld, t);

		//スフィアコライダーの座標をOBBのローカル空間にする
		Vector3 sphereLocal = Transform(spehrePos, inverseM_);

		//Sphere作成
		Sphere sphere = {sphereLocal,collider->GetRadius() };

		//当たり判定処理
		Vector3 saikin{};
		if (InCollision(aabb_, sphere, saikin)) {

			//ワールド空間に修正
			saikin = Transform(saikin, OBBM_);

			//値が同じ場合の処理
			if (sphereLocal == saikin) {
				//過去位置に向けてのベクトル算出
				Vector3 CP2EndP = preCWorld - saikin;
				//半径の長さ分押し戻す
				CP2EndP = CP2EndP.GetNormalizeNum() * collider->GetRadius();

				//worldに修正したものを渡す
				backVec = Transform(CP2EndP, OBBM_);

				////最近接点描画
				//DrawClosestP(Transform(saikin, OBBM_));

				//ヒットカラー処理
				SetColor(true);
				collider->SetColor(true);

				return true;
			}

			//最近接点から過去位置に向けての向きベクトルを取得
			Vector3 closestP2PreP = spehrePos - saikin;
			closestP2PreP.SetNormalize();

			//OBBローカルのCWorld
			Vector3 loCW = Transform(CWorld, inverseM_);
			//終点の最近接点を求める
			Vector3 nowCP = GetClosestPoint(loCW, aabb_.minV, aabb_.maxV);

			//ワールド座標系に修正
			nowCP = Transform(nowCP, OBBM_);
			//最近接点と現在位置との向きを求める
			Vector3 closestP2NowP = CWorld - nowCP;

			//もし終点がBox内の場合
			if (closestP2NowP == Vector3{0, 0, 0}) {
				//ここで速度過多による貫通処理を書く
			}

			//内積を求める
			float dot = closestP2PreP * closestP2NowP.GetNormalizeNum();
			getDot = dot;

			//長さを求める
			//最近接点と終点の距離
			float lengthEndP = (CWorld - nowCP).GetLength();

			//最大押し出し量を求める
			Vector3 backV = closestP2PreP * collider->GetRadius();
			//最近接点から移動位置までのベクトルを求める
			Vector3 diffV = closestP2PreP * lengthEndP;

			//ある程度同じ方向を向いてる場合
			if (dot >= 0) {
				//最大押し出し量に押し出し量
				backVec = backV - diffV;
			}
			else {
				backVec = backV + diffV;
			}


			////最近接点描画
			//DrawClosestP(Transform(saikin, OBBM_));

			//ヒットカラー処理
			SetColor(true);
			collider->SetColor(true);

			return true;



		}

		t += addNum;

		if (t > 1.0f) {
			//最近接点描画
			//OBBLocalPosCange
			//DrawClosestP(Transform(saikin, OBBM_));


			//色の変更
			SetColor(false);
			collider->SetColor(false);

			return false;
		}
	}
}

bool OBBCollider::IsCollision(const Segment& seg) {



	//各OBBローカル上に作成
	Vector3 localOri = Transform(seg.origin, inverseM_);
	Vector3 localEnd = Transform(seg.origin + seg.diff, inverseM_);

	//セグメント取得
	Segment lSeg = { localOri,localEnd - localOri };

	return InCollision(aabb_, seg);
}


void OBBCollider::SetColor(bool hit)
{
	if (hit) {
		color_ = hitColor;
	}
	else {
		color_ = normalColor;
	}
}

void OBBCollider::DrawClosestP(const Vector3& pos)
{
	EulerWorldTransform sWo;
	sWo.translate_ = pos;
	sWo.scale_ = { 0.1f,0.1f,0.1f };
	sWo.UpdateMatrix();
	IMM_->SetData("sphere", sWo);
}

void OBBCollider::UpdateMatrix()
{
	world_.UpdateMatrix();

#pragma region OBBのワールド行列をスケールなしで作成
	//回転量取得
	Vector3 rotate = GetAllRotate(world_);

	//回転行列
	Matrix4x4 rotateM = MakeRotateXMatrix(rotate.x) * (MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z));
	//座標行列
	Matrix4x4 translateM = MakeTranslateMatrix(world_.GetWorldTranslate());
	//スケールは使わない（sizeで使う
	Matrix4x4 scaleM = MakeIdentity4x4();
	//OBBのworld行列生成
	OBBM_ = scaleM * (rotateM * translateM);

	//逆行列
	inverseM_ = Inverse(OBBM_);

	//すべてのスケールからサイズ取得
	Vector3 size = GetAllScale(world_);

	//AABB取得
	aabb_ = { .minV = -size,.maxV = size };
#pragma endregion
}
