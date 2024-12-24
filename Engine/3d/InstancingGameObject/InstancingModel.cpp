#include "InstancingModel.h"

#include"Log/Log.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"TextureManager/TextureManager.h"
#include"SingleModelManager/ModelManager.h"
#include"functions/function.h"
#include"Log/Log.h"
#include"InstancingModelManager/InstancingModelManager.h"
#include"Camera/Camera.h"
#include"DeltaTimer/DeltaTimer.h"
#include"ImGuiManager/ImGuiManager.h"
#include<iostream>


InstancingModel::~InstancingModel() {

	//全解放処理
	modelData_.skinCluster.influenceResource->Release();
	modelData_.skinCluster.paletteResource->Release();

	indexResource_->Release();
	vertexResource_->Release();
	wvpResource_->Release();
	materialResource_->Release();
	directionalLightResource_->Release();
	cameraResource_->Release();
	pointlightResource_->Release();
}

InstancingModel* InstancingModel::CreateFromOBJ(const std::string& directory, const std::string& filePath, int instancingNum) {

	//フルパス作成
	std::string fileFullPath = directory + "/" + filePath + "/" + filePath + ".obj";
	//モデルデータ読み込み
	ModelAllData modeltea = LoadModelFile(directory, filePath);
	
	//作成
	InstancingModel* model = new InstancingModel();
	model->Initialize(modeltea, modeltea.model.material.textureFilePath, UINT(modeltea.model.vertices.size()), instancingNum);

	//ログに出力
	Log("Instancing Model " + filePath + " is Created!\n");

	return model;
}



void InstancingModel::AddInstancingData(const EulerWorldTransform& world, const Vector4& color) {
	//データをコピー
	InstancingData worl = { world,color };

	//追加
	instancingDatas_.push_back(std::move(worl));

}

void InstancingModel::UpdateAnimationCount()
{
	//
	if (nowAnimeName_ == "") {
		return;
	}

	if (modelType_ != kOBJModel) {
		if (isAnimationActive_) {
			//カウント進行
			animationTime_ += animationRoopSecond_ *(float)DeltaTimer::deltaTime_;

			Animation& anime = modelData_.animation[nowAnimeName_];

			//ループ時の処理
			if (isAnimeRoop_) {
				animationTime_ = std::fmod(animationTime_, anime.duration);//最後まで行ったら最初からリピート再生
			}
			else {
				if (animationTime_ > anime.duration) {
					animationTime_ = anime.duration;
				}
			}

			//マイナス領域の時の処理
			if (animationRoopSecond_ < 0) {
				if (animationTime_ < 0) {
					//ループ時の処理
					if (isAnimeRoop_) {
						animationTime_ += anime.duration;
					}
					else {
						animationTime_ = 0;
					}
				}
			}
		}
	}
}

void InstancingModel::UpdateAnimationBone(const std::string& animeName)
{

	if (modelType_ != kOBJModel) {

		Animation& anime = modelData_.animation[animeName];

		if (isAnimationActive_) {

			//ボーンのあるモデルの場合
			if (modelType_ == kSkinningGLTF) {
				//animationの更新を行って骨ごとのローカル情報を更新
				ApplyAnimation(modelData_.skeleton, anime, animationTime_);
				//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
				Update(modelData_.skeleton);
				//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
				Update(modelData_.skinCluster, modelData_.skeleton);
			}
			else {
				//ないanimationモデルの場合
				NodeAnimation& rootNodeAnimation = anime.nodeAnimations[modelData_.model.rootNode.name];
				Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
				Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
				localM_ = MakeAffineMatrix(scale, rotate, translate);
			}
		}
		else {
			animationTime_ = 0;
			//animationの更新を行って骨ごとのローカル情報を更新
			ApplyAnimation(modelData_.skeleton, anime, animationTime_);
			//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
			Update(modelData_.skeleton);
			//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
			Update(modelData_.skinCluster, modelData_.skeleton);
		}
	}
}

void InstancingModel::Draw(int texture) {

	//アニメーションカウント進行
	UpdateAnimationCount();
	//カウントに合わせたボーン状態に変更
	UpdateAnimationBone(nowAnimeName_);

	//カメラのインスタンス取得
	Camera* camera = Camera::GetInstance();

	int index = 0;
	for (auto& data : instancingDatas_) {

		//行列を計算
		Matrix4x4 WVP = data.world.matWorld_ * camera->GetViewProjectionMatrix();;

		//ボーンアニメーション以外は動く
		if (modelType_ == kAnimationGLTF) {
			wvpData_[index].WVP = localM_ * WVP;
			wvpData_[index].World = localM_ * data.world.matWorld_;
			wvpData_[index].WorldInverseTranspose = Inverse(Transpose(wvpData_[index].World));

		}
		else {
			wvpData_[index].WVP = WVP;
			wvpData_[index].World = data.world.matWorld_;
			wvpData_[index].WorldInverseTranspose = Inverse(Transpose(data.world.matWorld_));
		}

		wvpData_[index].color = data.color;

		//animationのあるモデルなら
		if (modelType_ == kSkinningGLTF) {
			if (drawJoint_) {
				//ジョイントMの更新
				int i = 0;
				for (auto& jointW : modelData_.skeleton.joints) {
					Matrix4x4 world = jointW.skeletonSpaceMatrix;

					EulerWorldTransform newdata;
					newdata.matWorld_ = world;

					IMM_->SetData(jointMtag_, newdata, { 1,1,1,1 });

					i++;
				}
			}


		}

		index++;
	}

	//最初のインスタンス以上の場合
	if (index > instancingNum_) {
		tag_;
		//indexが初期作成量よりおおい
		assert(false);
	}

	//０ではないとき描画処理
	if (index > 0) {
		
		//各データをセット
		materialData_->uvTransform = uvWorld_.UpdateMatrix();
		cameraData_->worldPosition = camera->GetMainCamera().GetWorldTranslate();

		PointLight pl = LightManager::GetInstance()->GetPLight();
		DirectionalLight dl = LightManager::GetInstance()->GetDLight();

		pointLightData_->color = pl.color;
		pointLightData_->decay = pl.decay;
		pointLightData_->intensity = pl.intensity;
		pointLightData_->position = pl.position;
		pointLightData_->radius = pl.radius;

		directionalLightData_->color = dl.color;
		directionalLightData_->direction = dl.direction;
		directionalLightData_->intensity = dl.intensity;


		//オブジェクト以外ならスキニング処理
		if (modelType_ == kSkinningGLTF) {
			vertexBufferView_ = skinningCS_->PreDraw();
		}

		//ルートシグネチャとパイプライン
		IMM_->GetPSO()->PreDraw(fillMode_, blendMode_);

		uvWorld_.UpdateMatrix();
		materialData_->uvTransform = uvWorld_.matWorld_;



		DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
		DXF_->GetCMDList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
		//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
		DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		//マテリアルCBufferの場所を設定
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		//ディレクショナルライト
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
		//カメラ位置転送
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
		//ポイントライト
		DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(5, pointlightResource_->GetGPUVirtualAddress());

		//画像が未指定の場合
		if (setTexture_ == -1) {
			if (texture == -1) {
				DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, texture_);
			}
			else {
				//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
				DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
			}
		}
		else {
			DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(setTexture_));
		}
		//インスタンシング用GHandle設定
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(1, instancingHandle_);


		//描画！		
		DXF_->GetCMDList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.model.indices.size()), index, 0, 0, 0);


		skinningCS_->PostDraw();
	}

	//ワールドデータ削除
	instancingDatas_.clear();
}


void InstancingModel::Debug(const char* name)
{
	//デバッグ用UI表示
	name;
#ifdef _DEBUG
	bool uselight = materialData_->enableLighting;
	bool useHalfLam = materialData_->enableHalfLambert;
	bool useTex = materialData_->enableTexture;

	Vector4 color = materialData_->color;

	if (ImGui::BeginMenu(name)) {

		ImGui::Text("Material");
		ImGui::Checkbox("lighting", &uselight);
		ImGui::Checkbox("HalfLam", &useHalfLam);
		ImGui::Checkbox("texture", &useTex);
		ImGui::ColorEdit4("color", &color.x);

		ImGui::EndMenu();
	}
#endif // _DEBUG
}

const float InstancingModel::GetWorldNum()
{
	//登録されている数取得
	float ans = 0;
	ans = (float)instancingDatas_.size();

	return ans;
}

GlobalVariableTree& InstancingModel::CreateAndGetTree(const std::string& tree)
{

	//デバッグ用にツリーにパラメータをセットする
	tree_.name_ = tree;
	tree_.SetText("他のところでも使用している場合同時に変更されるため注意");
	tree_.SetTreeData(uvWorld_.GetDebugTree("UV"));
	tree_.SetMonitorValue("モデル", &drawModel_);
	tree_.SetMonitorValue("ジョイント", &drawJoint_);
	tree_.SetValue("統一色", &materialData_->color);
	tree_.SetValue("画像フラグ", &materialData_->enableTexture);
	tree_.SetValue("ライティング", &materialData_->enableLighting);
	tree_.SetValue("ハーフランバート", &materialData_->enableHalfLambert);

	//ツリー返却
	return tree_;
}

void InstancingModel::Initialize(
	ModelAllData modelData,
	std::string name,
	int point,
	int instancingNum
) {
	//DXFのインスタンス追加
	DXF_ = DirectXFunc::GetInstance();

	modelData_ = modelData;

	modelData_.skeleton = CreateSkeleton(modelData_.model.rootNode);
	modelData_.skinCluster = CreateSkinCluster(*DXF_->GetDevice(), modelData_.skeleton, modelData_.model);
	//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
	Update(modelData_.skinCluster, modelData_.skeleton);

	//ジョイントのMの作成
	IMM_ = InstancingModelManager::GetInstance();
	jointMtag_ = "sphere";

	//各データ受け渡し
	point_ = point;
	instancingNum_ = instancingNum;


	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(WorldTransformation) * instancingNum);
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおくtextureResource
	for (uint32_t index = 0; index < (uint32_t)instancingNum; ++index) {
		wvpData_[index].WVP = MakeIdentity4x4();
		wvpData_[index].World = MakeIdentity4x4();
		wvpData_[index].WorldInverseTranspose = MakeIdentity4x4();
		wvpData_[index].color = { 1,1,1,1 };
	}


	//頂点データ
	vertexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * point);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * point);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDatatea = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDatatea));
	std::memcpy(vertexDatatea, modelData_.model.vertices.data(), sizeof(VertexData) * point);

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
	materialData_->enablePhongReflection = false;
	materialData_->shininess = 20.0f;
	materialData_->enablePointLight = false;
#pragma endregion

#pragma region ライト
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

#pragma region テクスチャ関係
	//スプライトの指定がない場合
	if (name == "") {
		int tex = TextureManager::uvChecker_;
		texture_ = SRVM->GetTextureDescriptorHandle(tex);
	}
	else {
		//指定があった場合
		//一度読み込む処理を挟む
		TextureManager::LoadTex(name);
		texture_ = TextureManager::GetGPUHandle(name);
		//texture_ = SRVM->GetTextureDescriptorHandle(texture);
	}
#pragma endregion
#pragma region instancing関係のデータ
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
		instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
		instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		instancingDesc.Buffer.FirstElement = 0;
		instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		instancingDesc.Buffer.NumElements = instancingNum;
		instancingDesc.Buffer.StructureByteStride = sizeof(WorldTransformation);

		instancingHandle_ = SRVManager::CreateSRV(wvpResource_, instancingDesc).gpu;
	}
#pragma endregion

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

	//スキニング用の処理クラス生成と初期化
	skinningCS_ = std::make_unique<SkinningCS>();
	skinningCS_->Initialize(modelData_);
}

void InstancingModel::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
	//ジョイントのキーフレームから現トランスフォーム変更
	for (Joint& joint : skeleton.joints) {
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}
