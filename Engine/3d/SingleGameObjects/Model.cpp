#include "Model.h"

#include"functions/function.h"
#include"TextureManager/TextureManager.h"
#include"Log/Log.h"

#include"ImGuiManager/ImGuiManager.h"
#include"SRVManager/SRVManager.h"
#include"InstancingModelManager/InstancingModelManager.h"
#include"struct.h"
#include"Camera/Camera.h"
#include"DeltaTimer/DeltaTimer.h"

#define _USE_MATH_DEFINES
#include<math.h>
#include<sstream>
#include<cassert>
#include<fstream>


#pragma region モデル


Model::Model()
{
}

Model::~Model() {

	//modelData_.skinCluster.influenceResource->Release();
	//modelData_.skinCluster.paletteResource->Release();

	indexResource_->Release();
	vertexResource_->Release();
	wvpResource_->Release();
	materialResource_->Release();
	directionalLightResource_->Release();
	cameraResource_->Release();
	pointlightResource_->Release();

}


Model* Model::CreateFromOBJ(const std::string& filePath)
{

	//モデルデータ管理クラスのインスタンス取得
	ModelManager* mManager = ModelManager::GetInstance();
	//モデルデータを取得
	ModelAllData modeltea = mManager->GetModelData(filePath);

	//新しくポインタを作成
	Model* model = new Model();
	//初期化処理
	model->Initialize(modeltea, modeltea.model.material.textureFilePath, UINT(modeltea.model.vertices.size()));

	//ログを出力
	Log("Model " + filePath + " is Created!\n");

	//終了
	return model;
}

void Model::UpdateAnimation()
{

	if (nowAnimeName_ == "") {
		return;
	}

	//OBJではない
	if (modelType_ != kOBJModel) {
		//アニメーションフラグON
		if (isAnimationActive_) {
			//
			animationTime_ += animationRoopSecond_ * modelData_.animation[nowAnimeName_].duration * (float)DeltaTimer::deltaTime_;
			//アニメーションループフラグON
			if (isAnimeRoop_) {
				animationTime_ = std::fmod(animationTime_, modelData_.animation[nowAnimeName_].duration);//最後まで行ったら最初からリピート再生
			}
			else {
				//ループしない処理
				if (animationTime_ > modelData_.animation[nowAnimeName_].duration) {
					animationTime_ = modelData_.animation[nowAnimeName_].duration;
				}
			}

			//マイナス領域の時の処理
			if (animationRoopSecond_ < 0) {

				if (animationTime_ < 0) {
					//ループ処理
					if (isAnimeRoop_) {
						animationTime_ += modelData_.animation[nowAnimeName_].duration;
					}
					else {
						animationTime_ = 0;
					}
				}
			}

			//指定してた場合
			if (isSetATime_) {
				animationTime_ = Lerp(0, modelData_.animation[nowAnimeName_].duration, setAt_);
			}

			//ボーンのあるモデルの場合
			if (modelType_ == kSkinningGLTF) {
				//animationの更新を行って骨ごとのローカル情報を更新
				ApplyAnimation(modelData_.skeleton, modelData_.animation[nowAnimeName_], animationTime_,isSetATime_);
				//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
				Update(modelData_.skeleton);
				//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
				Update(modelData_.skinCluster, modelData_.skeleton);
			}
			else {
				//ないanimationモデルの場合
				NodeAnimation& rootNodeAnimation = modelData_.animation[nowAnimeName_].nodeAnimations[modelData_.model.rootNode.name];
				Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
				Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
				localM_ = MakeAffineMatrix(scale, rotate, translate);
			}
		}
		else {
			animationTime_ = 0;
			//animationの更新を行って骨ごとのローカル情報を更新
			ApplyAnimation(modelData_.skeleton, modelData_.animation[nowAnimeName_], animationTime_,isSetATime_);
			//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
			Update(modelData_.skeleton);
			//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
			Update(modelData_.skinCluster, modelData_.skeleton);
		}
	}

}

Matrix4x4 Model::GetJoint(const std::string& name)
{
	for (auto& jointW : modelData_.skeleton.joints) {

		if (jointW.name == name) {
			return jointW.skeletonSpaceMatrix*wvpData_->World;
		}

	}
	assert(false);
	return Matrix4x4{};
}

void Model::Initialize(
	ModelAllData data,
	std::string name,
	int point
) {

	DXF_ = DirectXFunc::GetInstance();

	modelM_ = ModelManager::GetInstance();

	modelData_ = data;

	modelData_.skeleton = CreateSkeleton(modelData_.model.rootNode);
	
	modelData_.skinCluster = CreateSkinCluster(*DXF_->GetDevice(), modelData_.skeleton, modelData_.model);
	//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
	Update(modelData_.skinCluster, modelData_.skeleton);


	//ジョイントのMの作成
	jointM__ = InstancingModelManager::GetInstance();
	jointMtag_ = "sphere";


	//頂点データ
	vertexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * modelData_.model.vertices.size());
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.model.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.model.vertices.data(), sizeof(VertexData) * modelData_.model.vertices.size());


	//各データ受け渡し
	point_ = point;
	
	


	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む

	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおくtextureResource
	wvpData_->WVP = MakeIdentity4x4();
	wvpData_->World = MakeIdentity4x4();
	wvpData_->WorldInverseTranspose = MakeIdentity4x4();
	wvpData_->color = { 1,1,1,1 };

	//インデックス
	indexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(uint32_t) * modelData_.model.indices.size());
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * (uint32_t)modelData_.model.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* mappedIndex;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData_.model.indices.data(), sizeof(uint32_t) * modelData_.model.indices.size());


#pragma region マテリアル
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableTexture = true;
	materialData_->enableHalfLambert = true;
	materialData_->discardNum = 0.0f;
	materialData_->enablePhongReflection = 0;
	materialData_->shininess = 20.0f;
#pragma endregion

#pragma region ディレクションライト
	//ディレクションライトのマテリアルリソース
	directionalLightResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLightData_->direction = Vector3(0.0f, -1.0f, 0.0f);
	directionalLightData_->intensity = 1.0f;
#pragma endregion

#pragma region カメラ関係
	cameraResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(Camera4GPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = { 0,0,0 };
#pragma endregion

#pragma region ポイントライト
	pointlightResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(PointLight));
	pointlightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1,1,1,1 };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 1.0f;
	pointLightData_->decay = 1.0f;
#pragma endregion

	localM_ = MakeIdentity4x4();

	//スプライトの指定がない場合
	if (name == "") {
		int tex = TextureManager::uvChecker_;
		texture_ = TextureManager::GetGPUHandle(tex);
	}
	else {
		//指定があった場合
		texture_ = TextureManager::GetGPUHandle(TextureManager::LoadTex(name));
	}


	//アニメーションデータやボーンからモデルの状態を予想して設定
	if (modelData_.animation.size() != 0) {
		if (modelData_.model.skinClusterData.size() != 0) {
			modelType_ = kSkinningGLTF;
		}
		else {
			modelType_ = kAnimationGLTF;
		}
	}
	else {
		modelType_ = kOBJModel;
	}

	skinningCS_ = std::make_unique<SkinningCS>();
	skinningCS_->Initialize(modelData_);
	//EndInitialize

	
}

void Model::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime,bool designation)
{
	//補完の処理
	float t = 0;
	if (isSupplementation_) {
		supplementationCount_+=(float)DeltaTimer::deltaTime_;
		t = supplementationCount_ / maxSupplementationCount_;
		//カウント最大の時
		if (supplementationCount_>=maxSupplementationCount_) {
			//Tを1.0にする
			t = 1.0f;
			isSupplementation_ = false;
		}
	}

	//各ジョイントの更新
	int i = 0;
	for (Joint& joint : skeleton.joints) {
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}

		//アニメーション変更によるフラグ処理ONの場合
		if (isSupplementation_&&designation) {

			joint.transform.translate_ = Lerp(savedT[i].translate_, joint.transform.translate_, t);
			joint.transform.rotate_ = Slerp(savedT[i].rotate_, joint.transform.rotate_, t);
			joint.transform.scale_ = Lerp(savedT[i].scale_, joint.transform.scale_, t);

		}

		i++;
	}
}


void Model::Draw(const Matrix4x4& worldMatrix, int texture)
{

	UpdateAnimation();
	//animationのあるモデルなら
	if (modelType_ == kSkinningGLTF) {

		if (drawJoint_) {
			//ジョイントMの更新
			int i = 0;
			for (auto& jointW : modelData_.skeleton.joints) {
				//QuaterinionWorldTransform trans;;
				Matrix4x4 world = jointW.skeletonSpaceMatrix;

				EulerWorldTransform scaleW;
				scaleW.SetScale(0.1f);
				scaleW.UpdateMatrix();

				EulerWorldTransform newdata;

				newdata.matWorld_ = scaleW.matWorld_*  world * worldMatrix;

				jointM__->SetData(jointMtag_, newdata, { 1,1,1,1 });

				i++;
			}
		}
	}

	if (drawModel_) {
		//各データ確認用においてるだけ
		//modelData_;
		//skeleton_;
		//skinCluster_;

		materialData_->uvTransform = uvWorld_.UpdateMatrix();

		Camera* camera = Camera::GetInstance();

		Matrix4x4 WVP = worldMatrix * camera->GetViewProjectionMatrix();

		if (modelType_ == kAnimationGLTF) {
			wvpData_->WVP = localM_ * WVP;
			wvpData_->World = localM_ * worldMatrix;
			wvpData_->WorldInverseTranspose = Inverse(Transpose(wvpData_->World));
		}
		else {
			wvpData_->WVP = WVP;
			wvpData_->World = worldMatrix;
			wvpData_->WorldInverseTranspose = Inverse(Transpose(worldMatrix));

		}
		
		//オブジェクト以外ならスキニング処理
		if (modelType_ ==kSkinningGLTF) {
			vertexBufferView_ = skinningCS_->PreDraw();
		}
		//描画準備
		ModelManager::PreDraw( blendMode_, fillMode_);

	

		DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

		cameraData_->worldPosition = camera->GetMainCamera().GetWorldTranslate();

		PointLight pointlight = LightManager::GetInstance()->GetPLight();

		pointLightData_->color = pointlight.color;
		pointLightData_->position = pointlight.position;
		pointLightData_->radius = pointlight.radius;
		pointLightData_->intensity = pointlight.intensity;
		pointLightData_->decay = pointlight.decay;
		
		DirectionalLight dLight = LightManager::GetInstance()->GetDLight();
		directionalLightData_->color = dLight.color;
		directionalLightData_->direction = dLight.direction;
		directionalLightData_->intensity = dLight.intensity;

		DXF_->GetCMDList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
		//wvp用のCBufferの場所の設定
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
		//マテリアルCBufferの場所を設定
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		//
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
		//カメラ位置転送
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(5, pointlightResource_->GetGPUVirtualAddress());

		if (texture == -1) {
			DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, texture_);
		}
		else {
			//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
			DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
		}
		
		//仮で環境マップ画像を設定
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(6, TextureManager::GetGPUHandle(TextureManager::LoadTex("resources/Texture/CG/rostock_laage_airport_4k.dds")));

		//描画！		
		DXF_->GetCMDList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.model.indices.size()), 1, 0, 0, 0);
	}

	//描画後スキニング処理
	skinningCS_->PostDraw();
}



void Model::ChangeAnimation(const std::string& animeName, float sec)
{
	//同じものならスキップ
	if (animeName == nowAnimeName_) {
		return;
	}

	if (modelData_.animation.find(animeName) != modelData_.animation.end()) {
		nowAnimeName_ = animeName;
		if (sec != 0) {
			//補完フラグON
			isSupplementation_ = true;

			//各ジョイント位置保存
			savedT.clear();
			for (Joint& joint : modelData_.skeleton.joints) {
				QuaterinionWorldTransform newd;
				newd = joint.transform;
				savedT.emplace_back(newd);
			}

			//
			supplementationCount_ = 0;
			maxSupplementationCount_ = sec;
		}
		else {

		}

		animationTime_ = 0;
	}
	else {
	//見つからなかったのでエラー処理
	assert(false);
	}

}


GlobalVariableTree& Model::SetDebugParam(const std::string& treeName)
{
	tree.name_ = treeName;
	tree.SetValue("model", &drawModel_);
	tree.SetValue("joint", &drawJoint_);
	tree.SetValue("material color", &materialData_->color);
	tree.SetValue("テクスチャ", &materialData_->enableTexture);
	tree.SetValue("ディレクショナルライト", &materialData_->enableLighting);
	tree.SetValue("ハーフランバート", &materialData_->enableHalfLambert);
	
	tree.SetValue("ポイントライト", &materialData_->enablePointLight);
	tree.SetValue("キキャク", &materialData_->discardNum);
	tree.SetValue("Phong Reflection", &materialData_->enablePhongReflection);
	tree.SetValue("shiness", &materialData_->shininess);

	tree.SetValue("金属光沢",&materialData_->enableEnvironmentMap);
	tree.SetValue("光沢度", &materialData_->enviromentCoefficient);

	GlobalVariableTree uvTree = GlobalVariableTree("UV");
	uvTree.SetValue("座標", &uvWorld_.translate_);
	uvTree.SetValue("回転", &uvWorld_.rotate_);
	uvTree.SetValue("サイズ", &uvWorld_.scale_);

	GlobalVariableTree animation = GlobalVariableTree("アニメーション");
	animation.SetMonitorCombo("アニメーション", &nowAnimeName_, modelData_.name);
	animation.SetMonitorValue("アニメーションの自動再生フラグ", &isSetATime_);
	animation.SetMonitorValue("アニメーションの補間フラグ", &isSupplementation_);

	animation.SetValue("アニメーション進行度", &setAt_);
	animation.SetValue("有効", &isAnimationActive_);
	animation.SetValue("ループ", &isAnimeRoop_);
	animation.SetValue("ループ時間", &animationRoopSecond_);
	

	tree.SetTreeData(uvTree);
	tree.SetTreeData(animation);

	return tree;
}
#pragma endregion


