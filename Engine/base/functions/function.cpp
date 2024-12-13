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


//Vector3の遷移処理
Vector3 CalculateValue(const std::vector<KayframeVector3>& keyframes, float time) {
	//キーが空ならエラーを出す
	assert(!keyframes.empty());
	//サイズが１又は初期キーの時間が引数より少ないばあい
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		//そのまま返す
		return keyframes[0].value;
	}
	//キーフレームのサイズ分処理
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}


Quaternion CalculateValue(const std::vector<KayframeQuaternion>& keyframes, float time) {
	//キーがが空ならエラー
	assert(!keyframes.empty());
	//サイズが１又は初期キーの時間が引数より少ないばあい
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}
	//キーフレームのサイズ分取得
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}


std::map<std::string, Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename,std::vector<std::string>&names) {
	//返却する変数作成
	std::map<std::string, Animation> ans;

	//Assimpでの読み込み処理
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	//モデルのシーン読み込み
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	//アニメーションが０なら空で返却
	if (scene->mNumAnimations == 0) {
		return ans;
	}

	//アニメーションの数読み込み
	for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* animationAssimp = scene->mAnimations[i];//最初のアニメーションのみ読み込み
		Animation  result;
		std::string name = animationAssimp->mName.C_Str();

		result.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換
		//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報を取る
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			//参照渡し
			NodeAnimation& nodeAnimation = result.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			//ノードアニメーションのキーの数分座標取得
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KayframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手ー＞左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}

			//ノードアニメーションのキーの数分回転取得
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KayframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };//右手ー＞左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}

			//ノードアニメーションのキーの数分スケール取得
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KayframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手ー＞左手
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

		//アニメ名の所にデータを挿入
		ans[name]=result;

		//データ群に追加
		names.push_back(name);
	}

	return ans;
};



Node ReadNode(aiNode* node) {
	//編伽ky値
	Node result;

	//ノード名登録
	result.name = node->mName.C_Str();
	//子のサイズを設定
	result.children.resize(node->mNumChildren);
	//子のノードを読み込む
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	//トランスフォーム
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);//assimp行列からSRTを抽出する関数を利用
	//各値設定
	result.transform.scale_ = { scale.x,scale.y,scale.z };
	result.transform.rotate_ = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.transform.translate_ = { -translate.x,translate.y,translate.z };
	//行列を更新
	result.transform.UpdateMatrix();
	//出た結果をローカル行列とする
	result.localMatrix = result.transform.matWorld_;

	return result;
}

void Update(Skeleton& skeleton) {

	//スケルトン行列の更新
	for (Joint& joint : skeleton.joints) {
		//ジョイントの行列更新
		joint.transform.UpdateMatrix();
		//ローカル行列更新
		joint.localMatrix = joint.transform.matWorld_;
		//親がいる場合
		if (joint.parent) {
			//親を含めた行列計算を行う
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {//親がいないのでlocalMとskeletonは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

/// <summary>
/// ジョイントの作成
/// </summary>
/// <param name="node">ノード情報</param>
/// <param name="parent">親</param>
/// <param name="joints">ジョイントデータ</param>
/// <returns></returns>
int32_t CreateJoint(const Node& node,
	const std::optional<int32_t>& parent,
	std::vector<Joint>& joints) {
	//ジョイント作成
	Joint joint;
	//名前設定
	joint.name = node.name;
	//ノードのローカルをジョイントのローカルに
	joint.localMatrix = node.localMatrix;
	//アイデンティティ作成
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	//ノードのワールドを渡す
	joint.transform = node.transform;
	//登録され輝和をIndexに
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);
	for (const Node& child : node.children) {
		//子ジョイントを作成しそのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		//データを追加
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

Skeleton CreateSkeleton(const Node& node) {
	//スケルトン定義
	Skeleton skeleton;
	//ジョイントからスケルトンノード作成
	skeleton.root = CreateJoint(node, {}, skeleton.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	//更新処理
	Update(skeleton);

	return skeleton;
}

SkinCluster CreateSkinCluster(ID3D12Device& device, const Skeleton& skeleton, const ModelData& modelData) {
	//データを作成
	SkinCluster skinCluster;
	//使っていないSRVのハンドル取得
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
	//リソースの作成
	device.CreateShaderResourceView(skinCluster.paletteResource, &paletteSRVDesc, skinCluster.paletteSrvHandle.first);
#pragma endregion

#pragma region Influence用Resourceの作成
	//influence用のResourceを確保。頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResource = CreateBufferResource(&device, sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mapedInfluence = { mappedInfluence,modelData.vertices.size() };
	Handles ihandles = SRVManager::GetInstance()->CreateNewSRVHandles();
	skinCluster.influenceSrvHandle.first = ihandles.cpu;
	skinCluster.influenceSrvHandle.second = ihandles.gpu;

	//influence用のSRVデスク
	D3D12_SHADER_RESOURCE_VIEW_DESC iSRVDesc{};
	iSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	iSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	iSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	iSRVDesc.Buffer.FirstElement = 0;
	iSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	iSRVDesc.Buffer.NumElements = UINT(modelData.vertices.size());
	iSRVDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);
	
	//リソースの作成
	device.CreateShaderResourceView(skinCluster.influenceResource, &iSRVDesc, skinCluster.influenceSrvHandle.first);

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
	//スケルトンデータからクラスタデータ更新
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
		modeldata.animation = LoadAnimationFile(directoryPath, modelName,modeldata.name);
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

	//ノード解析
	modeldata.model.rootNode = ReadNode(scene->mRootNode);



	return modeldata;

}

ID3D12Resource* CreateUAVBufferResource(ID3D12Device* device, size_t sizeInBytes) {
#pragma region VertexResourceを生成する
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	
	//頂点リソースの設定
	D3D12_RESOURCE_DESC ResorceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResorceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResorceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは１にする決まり
	ResorceDesc.Height = 1;
	ResorceDesc.DepthOrArraySize = 1;
	ResorceDesc.MipLevels = 1;
	ResorceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResorceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ResorceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
#pragma endregion

	//頂点リソース作成
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr;
	hr = device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResorceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

#pragma region VertexResourceを生成する
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
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
	//頂点リソース
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr;
	hr = device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResorceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
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

	HRESULT hr;
	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
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
	//リソース
	ID3D12Resource* resource = nullptr;
	HRESULT hr;
	hr = device->CreateCommittedResource(
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
	//ヒープの開始地点取得
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//サイズ分ずらした位置を取得
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	//ヒープの開始地点取得
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//サイズ分ずらした位置を取得
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

