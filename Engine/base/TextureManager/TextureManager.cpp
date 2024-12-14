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
		//ddsファイルの場合特別処理
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		//それ以外はこっち
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));
	

	//ミップイメージ作成
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
	HRESULT hr;
	hr = device->CreateCommittedResource(
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
	//
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//バリア設定
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

//画像番号の初期化
int TextureManager::uvChecker_ = -1;

int TextureManager::white_ = -1;

#pragma region インスタンスと初期化
TextureManager* TextureManager::GetInstance()
{
	//インスタンス取得
	static TextureManager instance;
	return &instance;
}
void TextureManager::Initialize(DirectXFunc* DXF_)
{	
	//DXFポインタ取得
	DXF = DXF_;

	//仮画像読み込み
	uvChecker_ = TextureManager::LoadTex(uvCheckerTex).texNum;

	white_ = TextureManager::LoadTex("resources/Texture/SystemResources/white.png").texNum;

	//ログ出力
	Log("Complete TextureManager Initialize\n");

}

void TextureManager::Finalize() {

	//データをクリアする
	tagNumDatas_.clear();
	texDatas_.clear();
}
#pragma endregion

ReturnData TextureManager::LoadTex(const std::string& filePath)
{
	//パスがすでに呼ばれているかチェック
	if (!TextureManager::GetInstance()->CheckSameData(filePath)) {
		//呼ばれていない場合
		//画像作製
		DirectX::ScratchImage mipImages = LoadTexture(filePath);

		//入れた画像の管理番号を返す
		return TextureManager::GetInstance()->CreateData(filePath, mipImages);		
	}
	else {
		//呼ばれている場合
		//画像管理データ送信
		return TextureManager::GetInstance()->GetDataFromPath(filePath);
	}
}

ReturnData TextureManager::LoadTexShortPath(const std::string& filePath)
{

	//画像フォルダまでのパス
	std::string texfoldaPath = "resources/Texture/";

	//フルパス作成
	std::string fullpath = texfoldaPath + filePath;

	//パスがすでに呼ばれているかチェック
	if (!TextureManager::GetInstance()->CheckSameData(fullpath)) {
		//MIP画像作成
		DirectX::ScratchImage mipImages = LoadTexture(fullpath);

		//入れた画像の管理番号を返す
		return TextureManager::GetInstance()->CreateData(fullpath, mipImages);
	}
	else {
		//呼ばれている場合
		return TextureManager::GetInstance()->GetDataFromPath(fullpath);
	}
}


ReturnData TextureManager::CreateData(const std::string& filePath,const DirectX::ScratchImage& mipImages) {

	//SRVマネージャん取得
	SRVManager* SRVM = SRVManager::GetInstance();

	//データ作成
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

	//データを探索
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

	//データを探索
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














