#include "TextureManager.h"
#include"functions/function.h"
#include"Log/Log.h"
#include"SRVManager/SRVManager.h"


#pragma region 関数
//Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));
	

	/**/
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {//圧縮フォーマットなのか調べる
		mipImages = std::move(image);//圧縮じょーまっとならそのまま使うのでmoveする
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}
	assert(SUCCEEDED(hr));
	

	//みっぷマップ月のデータを返す
	return mipImages;

}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
#pragma region 1.metadataをもとにResourceの設定
	//metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									//Textureの幅
	resourceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);						//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);					//奥行き　or 配列Textureの配列数
	resourceDesc.Format = metadata.format;										//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											//サンプリングカウント１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		//Textureの次元数。普段使っているのは二次元
#pragma endregion
#pragma region 2.利用するHeapの設定
	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う

#pragma endregion
#pragma region 3.Resourceを生成する
	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,							//Heapの設定
		D3D12_HEAP_FLAG_NONE,						//Heapの特殊な設定。特になし。
		&resourceDesc,								//Reosurceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,				//データ転送される予定
		nullptr,									//Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&resource));					//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

#pragma endregion
	return resource;
}

[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}


#pragma endregion

int TextureManager::uvChecker_ = -1;

int TextureManager::white_ = -1;

#pragma region インスタンスと初期化
TextureManager* TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}
void TextureManager::Initialize(DirectXFunc* DXF_)
{	
	DXF = DXF_;

	uvChecker_ = TextureManager::LoadTex(uvCheckerTex).texNum;

	white_ = TextureManager::LoadTex("resources/Texture/SystemResources/white.png").texNum;

	Log("Complete TextureManager Initialize\n");

}

void TextureManager::Finalize() {
	tagNumDatas_.clear();
	texDatas_.clear();
}


#pragma endregion







ReturnData TextureManager::LoadTex(const std::string& filePath)
{
	DirectXFunc*DXF= DirectXFunc::GetInstance();

	//パスがすでに呼ばれているかチェック
	if (!TextureManager::GetInstance()->CheckSameData(filePath)) {

		DirectX::ScratchImage mipImages = LoadTexture(filePath);

		

		//入れた画像の管理番号を返す
		return TextureManager::GetInstance()->CreateData(filePath, mipImages);
		
	}
	else {
		//呼ばれている場合

		return TextureManager::GetInstance()->GetDataFromPath(filePath);
	}
}


ReturnData TextureManager::CreateData(const std::string& filePath,const DirectX::ScratchImage& mipImages) {

	SRVManager* SRVM = SRVManager::GetInstance();

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(DXF->GetDevice(), metadata);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, DXF->GetDevice(), DXF->GetCMDList());

	//metadataにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if(metadata.IsCubemap()){
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}
	Handles texNum= SRVM->CreateTextureSRV(textureResource, intermediateResource, srvDesc);

	Texturedata texData = { texNum.textureNum,filePath };

	ReturnData newdata = {
		texNum.gpu,
		texNum.textureNum
	};
	//データをプッシュ
	tagNumDatas_[filePath] = newdata;
	texDatas_[texNum.textureNum] = &texData;

	Log("Texture " + filePath + " is Loaded!\n");

	return newdata;
}

bool TextureManager::CheckSameData(const std::string& filepath) {
	//同じデータか確認
	/*for (auto& data : datas_) {
		if (data->filePath == filepath) {
			return true;
		}
	}*/

	auto it = tagNumDatas_.find(filepath);

	if (it != tagNumDatas_.end()) {
		//見つかった場合
		return true;
	}
	else {
		//無ければ
		return false;
	}


}

ReturnData TextureManager::GetDataFromPath(const std::string& path) {
	auto it = tagNumDatas_.find(path);

	if (it != tagNumDatas_.end()) {
		//見つかった場合
		return tagNumDatas_[path];
	}
	else {
	//見つからないのはおかしいのでエラー
	assert(false);
	return ReturnData();
	}

	
}














