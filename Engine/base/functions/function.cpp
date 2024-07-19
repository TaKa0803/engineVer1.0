#include"function.h"
#include"Log/Log.h"
#include"SRVManager/SRVManager.h"

#include<cassert>
#include<sstream>
#include<fstream>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

Vector3 CalculateValue(const std::vector<KayframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Esing(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Quaternion CalculateValue(const std::vector<KayframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}


std::vector<Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	std::vector<Animation> ans;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	if (scene->mNumAnimations == 0) {
		return ans;
	}

	for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* animationAssimp = scene->mAnimations[i];//最初のアニメーションのみ読み込み
		Animation  result;
		result.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換

		//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報を取る
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			//参照渡し
			NodeAnimation& nodeAnimation = result.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KayframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手ー＞左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}

			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KayframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };//右手ー＞左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KayframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手ー＞左手
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

		ans.emplace_back(result);
	}

	return ans;
}

Node ReadNode(aiNode* node) {
	Node result;
	//aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	//aiLocalMatrix.Transpose();
	//result.localMatrix.m[0][0] = aiLocalMatrix[0][0];
	//result.localMatrix.m[1][0] = aiLocalMatrix[1][0];
	//result.localMatrix.m[2][0] = aiLocalMatrix[2][0];
	//result.localMatrix.m[3][0] = aiLocalMatrix[3][0];

	//result.localMatrix.m[0][1] = aiLocalMatrix[0][1];
	//result.localMatrix.m[1][1] = aiLocalMatrix[1][1];
	//result.localMatrix.m[2][1] = aiLocalMatrix[2][1];
	//result.localMatrix.m[3][1] = aiLocalMatrix[3][1];

	//result.localMatrix.m[0][2] = aiLocalMatrix[0][2];
	//result.localMatrix.m[1][2] = aiLocalMatrix[1][2];
	//result.localMatrix.m[2][2] = aiLocalMatrix[2][2];
	//result.localMatrix.m[3][2] = aiLocalMatrix[3][2];

	//result.localMatrix.m[0][3] = aiLocalMatrix[0][3];
	//result.localMatrix.m[1][3] = aiLocalMatrix[1][3];
	//result.localMatrix.m[2][3] = aiLocalMatrix[2][3];
	//result.localMatrix.m[3][3] = aiLocalMatrix[3][3];

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	//トランスフォーム
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);//assimp行列からSRTを抽出する関数を利用
	result.transform.scale_ = { scale.x,scale.y,scale.z };
	result.transform.rotate_ = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.transform.translate_ = { -translate.x,translate.y,translate.z };

	result.transform.UpdateMatrix();

	result.localMatrix = result.transform.matWorld_;

	return result;
}

void Update(Skeleton& skeleton) {
	for (Joint& joint : skeleton.joints) {
		joint.transform.UpdateMatrix();
		joint.localMatrix = joint.transform.matWorld_;
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {//親がいないのでlocalMとskeletonは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

int32_t CreateJoint(const Node& node,
	const std::optional<int32_t>& parent,
	std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());//登録され輝和をIndexに
	joint.parent = parent;
	joints.push_back(joint);
	for (const Node& child : node.children) {
		//子ジョイントを作成しそのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

Skeleton CreateSkeleton(const Node& node) {
	Skeleton skeleton;
	skeleton.root = CreateJoint(node, {}, skeleton.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	Update(skeleton);

	return skeleton;
}

SkinCluster CreateSkinCluster(ID3D12Device& device, const Skeleton& skeleton, const ModelData& modelData) {

	SkinCluster skinCluster;

	Handles handles = SRVManager::GetInstance()->CreateNewSRVHandles();
#pragma region MatrixPaletteの作成
	//palette用のResourceを確保
	skinCluster.paletteResource = CreateBufferResource(&device, sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };
	skinCluster.paletteSrvHandle.first = handles.cpu;
	skinCluster.paletteSrvHandle.second = handles.gpu;

	//palette用のSRVを作成StructuredBufferでアクセスできるように
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSRVDesc{};
	paletteSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSRVDesc.Buffer.FirstElement = 0;
	paletteSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSRVDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSRVDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device.CreateShaderResourceView(skinCluster.paletteResource, &paletteSRVDesc, skinCluster.paletteSrvHandle.first);
#pragma endregion

#pragma region Influence用Resourceの作成
	//influence用のResourceを確保。頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResource = CreateBufferResource(&device, sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mapedInfluence = { mappedInfluence,modelData.vertices.size() };

	//Influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);
#pragma endregion

#pragma region ModelDataを解析してInfluenceを埋める
	for (const auto& jointWeight : modelData.skinClusterData) {//ModelのSkinClusterの情報を解析
		auto it = skeleton.jointMap.find(jointWeight.first);//joint.firstはjoint名なので、Skeletonに対象となるjointが含まれているか判断
		if (it == skeleton.jointMap.end()) {//そんな名前のJointは存在しないので次
			continue;
		}
		//(*it).secondにはJointのIndexが入っているので該当のIndexのInverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mapedInfluence[vertexWeight.vertexIndex];//該当のVertexIndexのInfluence情報を参照
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {//空いている所に入れる
				if (currentInfluence.weights[index] == 0.0f) {//weight==0が開いている状態なのでその場所にweightとjointのindexを代入
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
#pragma endregion


	return skinCluster;
}

void Update(SkinCluster& skinCluster, const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

ModelAllData LoadModelFile(const std::string& directoryPath, const std::string& modelName) {
	ModelAllData modeldata;//構築するModelData

	//読み込み
	Assimp::Importer imp;
	std::string filePath = directoryPath + "/" + modelName;
	const aiScene* scene = imp.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

	//animation読み込み
	if (scene->mNumAnimations != 0) {
		modeldata.animation = LoadAnimationFile(directoryPath, modelName);
	}

	assert(scene->HasMeshes());//メッシュがないのは非対応
	//メッシュ解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		//中身の回析
		modeldata.model.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			//座標変換わすれず
			modeldata.model.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modeldata.model.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modeldata.model.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };

		}
		//Indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modeldata.model.indices.push_back(vertexIndex);
			}
		}

		//SkinCluster構築用のデータ取得を追加
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//Jointごとの格納領域をつくる
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modeldata.model.skinClusterData[jointName];

			//InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			//左手座標系のBindPoseMatrixを作る
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix({ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			//InverseBindPoseMatrixにする
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			//Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}
		}

		//for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
		//	aiFace& face = mesh->mFaces[faceIndex];
		//	assert(face.mNumIndices == 3);//三角形のみサポ
		//	//各頂点回析
		//	for (uint32_t element = 0; element < face.mNumIndices; ++element) {
		//		uint32_t vertexIndex = face.mIndices[element];
		//		aiVector3D& position = mesh->mVertices[vertexIndex];
		//		aiVector3D& normal = mesh->mNormals[vertexIndex];
		//		aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
		//		VertexData vertex;
		//		vertex.position = { position.x,position.y,position.z,1.0f };
		//		vertex.normal = { normal.x,normal.y,normal.z };
		//		vertex.texcoord = { texcoord.x,texcoord.y };

		//		vertex.position.x *= -1.0f;
		//		vertex.normal.x *= -1.0f;
		//		modeldata.model.vertices.push_back(vertex);
		//	}
		//}
	}
	//マテリアル解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modeldata.model.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

#pragma region ノード解析
	modeldata.model.rootNode = ReadNode(scene->mRootNode);
#pragma endregion


	return modeldata;

}


//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
//	//1中で必要になる変数の宣言
//	MaterialData materialdata;
//
//	std::string line;
//	//２ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename + "/" + filename + ".mtl");
//	assert(file.is_open());
//	//３実際にファイルを読みまてりあｌDataを構築
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		//
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			//連結してファイルパスにする
//			materialdata.textureFilePath = directoryPath + "/" + filename + "/" + textureFilename;
//		}
//	}
//	//４MaterialDataを返す
//	return materialdata;
//
//}
//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
//#pragma region 中で必要となる変数の宣言
//	ModelData modeldata;//構築するModelData
//	std::vector<Vector4> positions;//位置
//	std::vector<Vector3>normals;//法線
//	std::vector<Vector2>texcoords;//texture座標
//	std::string line;//ファイルからよんだ一行を格納するもの	
//#pragma endregion
//#pragma region ファイルを開く
//
//	std::ifstream file(directoryPath + "/" + filename + "/" + filename + ".obj");
//	assert(file.is_open());//開けなかったら止める
//#pragma endregion
//#pragma region 実際にファイルを読みModelDataを構築していく
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;//先頭の識別子を読む
//
//		if (identifier == "v") {
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			position.x *= -1.0f;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt") {
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoord.y = 1.0f - texcoord.y;
//			//texcoord.x = 1.0f - texcoord.x;
//			texcoords.push_back(texcoord);
//		}
//		else if (identifier == "vn") {
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normal.x *= -1.0f;
//
//			normals.push_back(normal);
//		}
//		else if (identifier == "f") {
//			//面は三角形限定その他は未対応
//			VertexData triangle[3];
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのIndexは位置//UV/法線で格納されているので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//
//				for (int32_t element = 0; element < 3; ++element) {
//					std::string index;
//					std::getline(v, index, '/');//区切りでインデクスを読んでいく
//					elementIndices[element] = std::stoi(index);
//
//
//				}
//				//要素へのIndexから、実際の要素の値を取得して頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				//VertexData vertex = { position,texcoord,normal };
//				//modeldata.vertices.push_back(vertex);
//
//				triangle[faceVertex] = { position,texcoord,normal };
//			}
//			modeldata.vertices.push_back(triangle[2]);
//			modeldata.vertices.push_back(triangle[1]);
//			modeldata.vertices.push_back(triangle[0]);
//		}
//		else if (identifier == "mtllib") {
//			//materialTemplateLibraryファイルの名前を変更する
//			std::string materialFilename;
//			materialFilename = filename;
//			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
//			modeldata.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//
//
//	}
//#pragma endregion
//#pragma region 4.ModelDataを返す
//	return modeldata;
//#pragma endregion
//}
//



ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes,D3D12_HEAP_PROPERTIES uploadHeapProperties) {

#pragma region VertexResourceを生成する
	//頂点リソース用のヒープの設定
	
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResorceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResorceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResorceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは１にする決まり
	vertexResorceDesc.Height = 1;
	vertexResorceDesc.DepthOrArraySize = 1;
	vertexResorceDesc.MipLevels = 1;
	vertexResorceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResorceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
#pragma endregion
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResorceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
#pragma region 生成するReosurceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;											//Textureの幅
	resourceDesc.Height = height;										//Textureの高さ
	resourceDesc.MipLevels = 1;											//mipmapの数
	resourceDesc.DepthOrArraySize = 1;									//奥行き　or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;									//サンプリングカウント、１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;		//DepthStencilとして使う通知

	//りようするHeapの設定
	D3D12_HEAP_PROPERTIES heapProperities{};
	heapProperities.Type = D3D12_HEAP_TYPE_DEFAULT;						//VRAM上に作る
#pragma region 深度地クリア最適化設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット、Resourceと合わせる
#pragma endregion
#pragma region Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperities,												//Heapの設定
		D3D12_HEAP_FLAG_NONE,											//Heapの特殊な設定、特になし
		&resourceDesc,													//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,								//深度値を書き込む状態にしておく
		&depthClearValue,												//Clear最適地
		IID_PPV_ARGS(&resource));										//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
#pragma endregion

#pragma endregion

}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

