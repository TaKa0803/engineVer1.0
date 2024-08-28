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
	preWorld_ = world_;
	/*Matrix4x4 rotateXM = MakeRotateAxisAngle({ 1,0,0 }, rotation_.x);
	Matrix4x4 rotateYM = MakeRotateAxisAngle({ 0,1,0 }, rotation_.y);
	Matrix4x4 rotateZM = MakeRotateAxisAngle({ 0,0,1 }, rotation_.z);

	Matrix4x4 rotateM = rotateXM * (rotateYM * rotateZM);

	Matrix4x4 scaleM = MakeScaleMatrix(world_.scale_);

	Matrix4x4 tlansM = MakeTranslateMatrix(world_.translate_);

	Matrix4x4 worldM = scaleM * (rotateM * tlansM);

	if (&world_.GetParent() != nullptr) {
		Matrix4x4 pare = world_.parent_->matWorld_;
		world_.matWorld_ =  worldM * pare;
	}
	else {
		world_.matWorld_ = worldM;
	}*/

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
	Matrix4x4 translateM = MakeTranslateMatrix(world_.GetMatWorldTranslate());
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

	float addNum =  1.0f/division;
	float t = 0;

	Vector3 preCWorld = collider->GetPreWorld().GetMatWorldTranslate();
	Vector3 CWorld = collider->GetWorld().GetMatWorldTranslate();

	//ローカル変換
	preCWorld = Transform(preCWorld, inverseM_);
	CWorld = Transform(CWorld, inverseM_);


	while (true) {

		//過去地点から現在地点まで段階的に探査する
		Vector3 spehrePos = Esing(preCWorld, CWorld, t);

		//スフィアコライダーの座標をOBBのローカル空間に出る
		//Vector3 sphereLocal = Transform(spehrePos, inverseM_);


		//Sphere取得
		Sphere sphere = {spehrePos,collider->GetRadius() };

		//当たり判定
		Vector3 saikin{};
		if (InCollision(aabb_, sphere, saikin)) {

			//値が同じ場合の処理
			if (spehrePos == saikin) {
				//過去位置に向けてのベクトル算出
				Vector3 CP2EndP = preCWorld - saikin;
				//半径の長さ分押し戻す
				CP2EndP = CP2EndP.GetNormalizeNum() * collider->GetRadius();

				//worldに修正したものを渡す
				backVec = Transform(CP2EndP, OBBM_);

				////最近接点描画
				EulerWorldTransform sWo;
				sWo.translate_ = Transform(saikin, OBBM_);
				sWo.scale_ = { 0.1f,0.1f,0.1f };
				sWo.UpdateMatrix();
				IMM_->SetData("sphere", sWo);
				//ヒットカラー処理
				SetColor(true);
				collider->SetColor(true);

				return true;
			}
			//OBBのローカル座標からworld座標に変換
			//saikin = Transform(saikin, OBBM_);


			///押し出しベクトルを利用して計算
			//最近接点から円の中心点への向きベクトルを算出
			//Vector3 velo = spehrePos - saikin;
			//正規化

			//Vector3 norVe = velo;
			//norVe.SetNormalize();
			//半径分伸ばす
			//norVe *= collider->GetRadius();
			//渡す
			//backVec = norVe - velo;

			//すべてローカル内処理
			//最近接点から過去位置に向けての向きベクトルを取得
			Vector3 closestP2PreP = spehrePos - saikin;
			closestP2PreP.SetNormalize();

			//終点の最近接点を求める
			Vector3 nowCP = GetClosestPoint(CWorld, aabb_.minV, aabb_.maxV);
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

			//worldに変換
			backVec = Transform(backVec,OBBM_);

			////最近接点描画
			EulerWorldTransform sWo;
			sWo.translate_ = Transform(saikin,OBBM_);
			sWo.scale_ = { 0.1f,0.1f,0.1f };
			sWo.UpdateMatrix();
			IMM_->SetData("sphere", sWo);
			//ヒットカラー処理
			SetColor(true);
			collider->SetColor(true);

			return true;



		}

		t += addNum;

		if (t > 1.0f) {
			//最近接点描画
			//OBBLocalPosCange
			saikin = Transform(saikin, OBBM_);
			EulerWorldTransform sWo;
			sWo.translate_ = saikin;
			sWo.scale_ = { 0.1f,0.1f,0.1f };
			sWo.UpdateMatrix();
			IMM_->SetData("sphere", sWo);


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

void OBBCollider::UpdateMatrix()
{
	world_.UpdateMatrix();

#pragma region OBBのワールド行列をスケールなしで作成
	//回転量取得
	Vector3 rotate = GetAllRotate(world_);

	//回転行列
	Matrix4x4 rotateM = MakeRotateXMatrix(rotate.x) * (MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z));
	//座標行列
	Matrix4x4 translateM = MakeTranslateMatrix(world_.GetMatWorldTranslate());
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
