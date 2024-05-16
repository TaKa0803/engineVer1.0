#include "Model.h"

#include"functions/function.h"
#include"TextureManager/TextureManager.h"
#include"Log/Log.h"

#include"ImGuiManager/ImGuiManager.h"
#include"SRVManager/SRVManager.h"

#include"struct.h"

#define _USE_MATH_DEFINES
#include<math.h>
#include<sstream>
#include<cassert>
#include<fstream>


#pragma region モデル


Model::~Model() {
	
	indexResource_->Release();
	vertexData_->Release();
	wvpResource_->Release();
	materialResource_->Release();
	directionalLightResource_->Release();
	cameraResource_->Release();
	pointlightResource_->Release();
}

Model* Model::CreateSphere(float kSubdivision,bool enableLighting, const std::string& filePath)
{
	DirectXFunc*DXF= DirectXFunc::GetInstance();

#pragma region 円
#pragma region VertexBufferViewを作成
	int point = (int)kSubdivision * (int)kSubdivision * 6;

	
	ID3D12Resource* vertexResourceSphere = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * point);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};

	//リソースの戦闘のアドレスから使う
	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferViewSphere.SizeInBytes = sizeof(VertexData) * point;
	//1頂点当たりのサイズ
	vertexBufferViewSphere.StrideInBytes = sizeof(VertexData);
#pragma endregion
#pragma region Resourceにデータを書き込む
	//時計周りに点を設定していく
	VertexData* vertexDataS = nullptr;
	//書き込むためのアドレスを取得
	vertexResourceSphere->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexDataS));

	//経度一つ分の角度δ
	const float kLonEvery = (float)M_PI * 2.0f / float(kSubdivision);
	//緯度一つ分の角度Θ
	const float kLatEvery = (float)M_PI / float(kSubdivision);
	//緯度の方向に分割
	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex) {//σ
		float lat = -(float)M_PI / 2.0f + kLatEvery * latIndex;
		//経度のほう
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {//Θ
			uint32_t start = ((int)latIndex * (int)kSubdivision + (int)lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			//一枚目
			//1(a)
			vertexDataS[start].position = { cos(lat) * cos(lon),sin(lat),  cos(lat) * sin(lon),1.0f };
			vertexDataS[start].texcoord = { (lonIndex) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexDataS[start].normal = { vertexDataS[start].position.x,vertexDataS[start].position.y,vertexDataS[start].position.z };

			//2(b)
			vertexDataS[start + 1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexDataS[start + 1].texcoord = { (lonIndex) / (float)kSubdivision,1.0f - (latIndex + 1) / (float)kSubdivision };
			vertexDataS[start + 1].normal = { vertexDataS[start + 1].position.x,vertexDataS[start + 1].position.y,vertexDataS[start + 1].position.z };

			//3(c)
			vertexDataS[start + 2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),  cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexDataS[start + 2].texcoord = { (lonIndex + 1) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexDataS[start + 2].normal = { vertexDataS[start + 2].position.x,vertexDataS[start + 2].position.y,vertexDataS[start + 2].position.z };

			//二枚目
			//
			vertexDataS[start + 3] = vertexDataS[start + 1];
			//
			vertexDataS[start + 4].position = { cos(lat + kLatEvery) * cos(lon + kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexDataS[start + 4].texcoord = { (lonIndex + 1) / (float)kSubdivision,1.0f - (latIndex + 1) / (float)kSubdivision };
			vertexDataS[start + 4].normal = { vertexDataS[start + 4].position.x,vertexDataS[start + 4].position.y,vertexDataS[start + 4].position.z };
			//
			vertexDataS[start + 5] = vertexDataS[start + 2];
		}
	}
#pragma endregion
#pragma region wvp設定
	ID3D12Resource* wvpResourceS;

	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	wvpResourceS = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpDataS = nullptr;

	//書き込むためのアドレスを取得
	wvpResourceS->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataS));
	//単位行列を書き込んでおくtextureResource
	wvpDataS->WVP = MakeIdentity4x4();
	wvpDataS->World = MakeIdentity4x4();
#pragma endregion	
#pragma endregion	


	ModelAllData modeldata;

	Model* model = new Model();
	model->Initialize(modeldata,filePath, point,vertexResourceSphere, vertexBufferViewSphere, wvpResourceS, wvpDataS);
	
	
	
	return model;

}

Model* Model::CreateFromOBJ(const std::string& filePath)
{
	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region モデル
	ModelManager*mManager= ModelManager::GetInstance();

	ModelAllData modeltea =mManager->GetModelData(filePath); 

	

	ID3D12Resource* vertexRtea = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * modeltea.model.vertices.size());
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewtea{};
	vertexBufferViewtea.BufferLocation = vertexRtea->GetGPUVirtualAddress();
	vertexBufferViewtea.SizeInBytes = UINT(sizeof(VertexData) * modeltea.model.vertices.size());
	vertexBufferViewtea.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDatatea = nullptr;
	vertexRtea->Map(0, nullptr, reinterpret_cast<void**>(&vertexDatatea));
	std::memcpy(vertexDatatea, modeltea.model.vertices.data(), sizeof(VertexData) * modeltea.model.vertices.size());

	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	ID3D12Resource* wvpResourceTea = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpDataTea = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceTea->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataTea));
	//単位行列を書き込んでおくtextureResource
	wvpDataTea->WVP = MakeIdentity4x4();
	wvpDataTea->World = MakeIdentity4x4();
#pragma endregion

	Model* model =new Model();
	model->Initialize(modeltea,modeltea.model.material.textureFilePath,UINT(modeltea.model.vertices.size()),vertexRtea, vertexBufferViewtea, wvpResourceTea, wvpDataTea);
	
	return model;
}

void Model::UpdateAnimation()
{

	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, modelData_.animation.duration);
	//animationの更新を行って骨ごとのローカル情報を更新
	ApplyAnimation(skeleton_, modelData_.animation,animationTime);
	//骨ごとのLocal情報をもとにSkeletonSpaceの情報更新
	Update(skeleton_);
	//SkeletonSpaceの情報をもとにSkinClusterのまｔりｘPaletteを更新
	Update(skinCluster_, skeleton_);
}

void Model::Initialize(
	ModelAllData data,
	std::string name_,
	int point,
	ID3D12Resource* vertexRtea,
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
	ID3D12Resource* wvpResource,
	WorldTransformation* wvpData
) {

	DXF_ = DirectXFunc::GetInstance();

	modelM_ = ModelManager::GetInstance();

	modelData_ = data;

	name =name_;

	skeleton_ = CreateSkeleton(modelData_.model.rootNode);

	Handles handles = SRVManager::GetInstance()->CreateNewSRVHandles();

	skinCluster_ = CreateSkinCluster(*DXF_->GetDevice(), skeleton_, modelData_.model, handles.cpu, handles.gpu);

	//ジョイントのMの作成
	jointM__ = InstancingModelManager::GetInstance();
	jointMtag_ = "sphere";

	SRVManager* SRVM = SRVManager::GetInstance();

	//スプライトの指定がない場合
	if (name_ == "") {
		int tex = TextureManager::uvChecker_;
		texture_ = SRVM->GetTextureDescriptorHandle(tex);
	}
	else {
		//指定があった場合
		int texture = TextureManager::LoadTex(name);	
		texture_ = SRVM->GetTextureDescriptorHandle(texture);
	}

	point_ = point;
	vertexData_ = vertexRtea;
	vertexBufferView_ = vertexBufferView;
	wvpData_ = wvpData;
	wvpResource_ = wvpResource;
	
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

	Log("Model " +name_ +" is Created!\n");
}


void Model::Draw(const Matrix4x4& worldMatrix, const Camera& camera,Vector3 pointlight, int texture)
{
	UpdateAnimation();

	
	materialData_->uvTransform = MakeAffineMatrix(uvscale, uvrotate, uvpos);

	Matrix4x4 WVP = worldMatrix * camera.GetViewProjectionMatrix();

	wvpData_->WVP = WVP;
	wvpData_->World = worldMatrix;

	bool isAnime = false;
	//animationのあるモデルなら
	if (modelData_.animation.nodeAnimations.size() != 0) {
		isAnime = true;

		if (drawJoint_) {
			//ジョイントMの更新
			int i = 0;
			for (auto& jointW : skeleton_.joints) {
				Matrix4x4 world = jointW.skeletonSpaceMatrix;

				EulerWorldTransform newdata;
				newdata.matWorld_ = world;

				jointM__->SetData(jointMtag_, newdata, { 1,1,1,1 });

				i++;
			}
		}
		

	}
	else {
		
	}


	ModelManager::PreDraw(isAnime,fillMode_, blendMode_);

	if (isAnime) {
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		vertexBufferView_,					//VertexDataのView
		skinCluster_.influenceBufferView	//InfluenceのVPV
		};

		DXF_->GetCMDList()->IASetVertexBuffers(0, 2, vbvs);
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(6, skinCluster_.paletteSrvHandle.second);
	}
	else {
		DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	}


	wvpData_->WorldInverseTranspose = Inverse(Transpose(worldMatrix));

	cameraData_->worldPosition = camera.GetMainCamera().GetMatWorldTranslate();

	pointLightData_->position = pointlight;

	
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
	if (drawModel_) {
		//描画！		
		DXF_->GetCMDList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.model.indices.size()), 1, 0, 0, 0);
	}
}





void Model::PlayAnimation(int animeNum)
{
	if (modelData_.animation.nodeAnimations.size() != 0) {
		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, modelData_.animation.duration);

		NodeAnimation& rootNodeAnimation = modelData_.animation.nodeAnimations[modelData_.model.rootNode.name];
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);



		localM_ = MakeAffineMatrix(scale, rotate, translate);

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
	const char* fitems[] = { "Solid","WireFrame"};
	int currentfItem = static_cast<int>(fill);


	float discardnum = materialData_->discardNum;

	bool usePhong = materialData_->enablePhongReflection;
	float shininess = materialData_->shininess;

	bool usePointLight = materialData_->enablePointLight;


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
		ImGui::DragFloat2("uv pos", &uvpos.x, 0.1f);
		ImGui::DragFloat("uv rotate", &uvrotate.z, 0.1f);
		ImGui::DragFloat2("uv scale", &uvscale.x, 0.1f);

		ImGui::Text("DirectionalLight");
		ImGui::DragFloat3("D light direction", &directionalLightData_->direction.x, 0.01f);
		ImGui::DragFloat("D light intensity", &directionalLightData_->intensity, 0.01f);
		ImGui::ColorEdit4("D light color", &directionalLightData_->color.x);
		
		ImGui::Text("Blinn Phong Reflection");
		ImGui::DragFloat("Shininess", &shininess);
		
		ImGui::Text("PointLight");
		ImGui::DragFloat("p light intencity", &pointLightData_->intensity,0.01f);
		ImGui::DragFloat3("p light pos", &pointLightData_->position.x, 0.1f);
		ImGui::DragFloat("p light radius", &pointLightData_->radius, 0.1f);
		ImGui::DragFloat("p light decay", &pointLightData_->decay, 0.1f);
		ImGui::EndMenu();
	}
	

	materialData_->enableTexture = useTexture;
	materialData_->enableLighting = useShader;
	materialData_->enableHalfLambert = useHalf;
	materialData_->color = color;
	blendMode_ = blend;
	fillMode_ = fill;
	materialData_->discardNum= discardnum;
	materialData_->enablePhongReflection = usePhong;
	materialData_->shininess = shininess;
	materialData_->enablePointLight = usePointLight;
#endif // _DEBUG
	
}
#pragma endregion


