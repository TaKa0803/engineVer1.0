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

	modelData_.skinCluster.influenceResource->Release();
	modelData_.skinCluster.paletteResource->Release();

	indexResource_->Release();
	vertexResource_->Release();
	wvpResource_->Release();
	materialResource_->Release();
	directionalLightResource_->Release();
	cameraResource_->Release();
	pointlightResource_->Release();

	skinningCS_.reset();
}


Model* Model::CreateFromOBJ(const std::string& filePath)
{
	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region モデル
	ModelManager* mManager = ModelManager::GetInstance();

	ModelAllData modeltea = mManager->GetModelData(filePath);


	

#pragma endregion

	Model* model = new Model();
	model->Initialize(modeltea, modeltea.model.material.textureFilePath, UINT(modeltea.model.vertices.size()));


	Log("Model " + filePath + " is Created!\n");

	return model;
}

void Model::UpdateAnimation()
{

	//OBJではない
	if (modelType_ != kOBJModel) {
		//アニメーションフラグON
		if (isAnimationActive_) {
			//
			animationTime_ += animationRoopSecond_ * modelData_.animation[animeNum_].duration * (float)DeltaTimer::deltaTime_;
			//アニメーションループフラグON
			if (isAnimeRoop_) {
				animationTime_ = std::fmod(animationTime_, modelData_.animation[animeNum_].duration);//最後まで行ったら最初からリピート再生
			}
			else {
				//ループしない処理
				if (animationTime_ > modelData_.animation[animeNum_].duration) {
					animationTime_ = modelData_.animation[animeNum_].duration;
				}
			}

			//マイナス領域の時の処理
			if (animationRoopSecond_ < 0) {

				if (animationTime_ < 0) {
					//ループ処理
					if (isAnimeRoop_) {
						animationTime_ += modelData_.animation[animeNum_].duration;
					}
					else {
						animationTime_ = 0;
					}
				}
			}

			//指定してた場合
			if (isSetATime_) {
				animationTime_ = Lerp(0, modelData_.animation[animeNum_].duration, setAt_);
			}

			//ボーンのあるモデルの場合
			if (modelType_ == kSkinningGLTF) {
				//animationの更新を行って骨ごとのローカル情報を更新
				ApplyAnimation(modelData_.skeleton, modelData_.animation[animeNum_], animationTime_);
				//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
				Update(modelData_.skeleton);
				//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
				Update(modelData_.skinCluster, modelData_.skeleton);
			}
			else {
				//ないanimationモデルの場合
				NodeAnimation& rootNodeAnimation = modelData_.animation[animeNum_].nodeAnimations[modelData_.model.rootNode.name];
				Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
				Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
				localM_ = MakeAffineMatrix(scale, rotate, translate);
			}
		}
		else {
			animationTime_ = 0;
			//animationの更新を行って骨ごとのローカル情報を更新
			ApplyAnimation(modelData_.skeleton, modelData_.animation[animeNum_], animationTime_);
			//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
			Update(modelData_.skeleton);
			//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
			Update(modelData_.skinCluster, modelData_.skeleton);
		}
	}

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

	SRVManager* SRVM = SRVManager::GetInstance();

	//スプライトの指定がない場合
	if (name == "") {
		int tex = TextureManager::uvChecker_;
		texture_ = SRVM->GetTextureDescriptorHandle(tex);
	}
	else {
		//指定があった場合
		texture_ = TextureManager::LoadTex(name).gpuHandle;
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

void Model::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
	//補完処理
	float t = 0;
	if (isSupplementation_) {
		supplementationCount_+=(float)DeltaTimer::deltaTime_;
		t = supplementationCount_ / maxSupplementationCount_;
		if (t >= 1.0f) {
			t = 1.0f;
			isSupplementation_ = false;
		}
	}

	int i = 0;
	for (Joint& joint : skeleton.joints) {
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}

		//アニメーション変更によるフラグ処理ONの場合
		if (isSupplementation_) {

			joint.transform.translate_ = Lerp(savedT[i].translate_, joint.transform.translate_, t);
			joint.transform.rotate_ = Slerp(savedT[i].rotate_, joint.transform.rotate_, t);
			joint.transform.scale_ = Lerp(savedT[i].scale_, joint.transform.scale_, t);

		}

		i++;
	}
}


void Model::Draw(const Matrix4x4& worldMatrix, int texture)
{

	//animationのあるモデルなら
	if (modelType_ == kSkinningGLTF) {

		if (drawJoint_) {
			//ジョイントMの更新
			int i = 0;
			for (auto& jointW : modelData_.skeleton.joints) {
				Matrix4x4 world = jointW.skeletonSpaceMatrix;

				EulerWorldTransform newdata;
				newdata.matWorld_ = world * wvpData_->World;

				jointM__->SetData(jointMtag_, newdata,0, { 1,1,1,1 });

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
		bool isAnime = false;
		//描画準備
		ModelManager::PreDraw(isAnime, blendMode_, fillMode_);

	

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
		
		//仮
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(6, TextureManager::LoadTex("resources/Texture/CG/rostock_laage_airport_4k.dds").gpuHandle);


		//描画！		
		DXF_->GetCMDList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.model.indices.size()), 1, 0, 0, 0);
	}

	skinningCS_->PostDraw();
}


void Model::ChangeAnimation(int animeNum, float count)
{

	//同じものならスキップ
	if (animeNum_ == animeNum) {
		return;
	}

	//アニメーションの値内ならアニメーション変更と補完処理フラグON
	if (animeNum <= modelData_.animation.size()) {
		animeNum_ = animeNum;

		if (count != 0) {
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
			maxSupplementationCount_ = count;
		}
		else {

		}

		animationTime_ = 0;
	}
}

void Model::DebugParameter(const char* name)
{
#ifdef _DEBUG
	bool useTexture = materialData_->enableTexture;
	bool useShader = materialData_->enableLighting;
	bool useHalf = materialData_->enableHalfLambert;
	Vector4 color = materialData_->color;

	BlendMode blend = blendMode_;
	const char* items[] = { "None","Normal","Add","Subtract","Multiply","Screen" };
	int currentItem = static_cast<int>(blend);

	FillMode fill = fillMode_;
	const char* fitems[] = { "Solid","WireFrame" };
	int currentfItem = static_cast<int>(fill);


	float discardnum = materialData_->discardNum;

	bool usePhong = materialData_->enablePhongReflection;
	float shininess = materialData_->shininess;

	bool usePointLight = materialData_->enablePointLight;

	int anum = animeNum_;

	bool EnvironmentTexture = materialData_->enableEnvironmentMap;

	if (ImGui::BeginMenu(name)) {
		ImGui::Checkbox("Texture", &useTexture);
		ImGui::Checkbox("Shader", &useShader);
		ImGui::Checkbox("HalfLambert", &useHalf);
		ImGui::Checkbox("PhongReflection", &usePhong);
		ImGui::Checkbox("enable pointlight", &usePointLight);
		ImGui::Checkbox("draw model", &drawModel_);
		ImGui::Checkbox("draw joint", &drawJoint_);

		ImGui::ColorEdit4("Material color", &color.x);
		if (ImGui::Combo("blendmode", &currentItem, items, IM_ARRAYSIZE(items))) {
			blend = static_cast<BlendMode>(currentItem);
		}
		if (ImGui::Combo("fillmode", &currentfItem, fitems, IM_ARRAYSIZE(fitems))) {
			fill = static_cast<FillMode>(currentfItem);
		}
		ImGui::DragFloat("discardNum", &discardnum, 0.01f);

		ImGui::Text("UV");
		ImGui::DragFloat2("uv pos", &uvWorld_.translate_.x, 0.1f);
		ImGui::DragFloat("uv rotate", &uvWorld_.rotate_.z, 0.1f);
		ImGui::DragFloat2("uv scale", &uvWorld_.scale_.x, 0.1f);

		ImGui::Text("Animation");
		ImGui::Checkbox("animeActive", &isAnimationActive_);
		ImGui::Checkbox("animeRoop", &isAnimeRoop_);
		ImGui::DragFloat("Roop second", &animationRoopSecond_, 0.1f);
		ImGui::SliderInt("AnimeNum", &anum, 0, (int)(modelData_.animation.size())-1);

		ImGui::Text("Blinn Phong Reflection");
		ImGui::DragFloat("Shininess", &shininess);

		ImGui::Checkbox("金属光沢処理", &EnvironmentTexture);
		ImGui::SliderFloat("光沢度", &materialData_->enviromentCoefficient, 0, 1);
		ImGui::EndMenu();
	}

	materialData_->enableEnvironmentMap = EnvironmentTexture;

	materialData_->enableTexture = useTexture;
	materialData_->enableLighting = useShader;
	materialData_->enableHalfLambert = useHalf;
	materialData_->color = color;
	blendMode_ = blend;
	fillMode_ = fill;
	materialData_->discardNum = discardnum;
	materialData_->enablePhongReflection = usePhong;
	materialData_->shininess = shininess;
	materialData_->enablePointLight = usePointLight;

	if (animeNum_ != anum) {
		ChangeAnimation(anum, 60);
	}
#endif // _DEBUG

}
#pragma endregion


