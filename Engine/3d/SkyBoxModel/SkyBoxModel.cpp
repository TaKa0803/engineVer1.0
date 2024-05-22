#include "SkyBoxModel.h"
#include"TextureManager/TextureManager.h"
#include"functions/function.h"

SkyBoxModel::SkyBoxModel(const std::string& texPath)
{
	DXF_ = DirectXFunc::GetInstance();

	pso_ = new SkyBoxPSO();
	pso_->Initialize();

	texture_ = TextureManager::GetInstance()->LoadTex(texPath).gpuHandle;

	vertexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * 8);
	
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * 8);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//前面
	//左上
	vertexData[0].position = { -1.0f,1.0f,1.0f };
	//右上
	vertexData[1].position = { 1.0f,1.0f,1.0f };
	//左下
	vertexData[2].position = { -1.0f,-1.0f,1.0f };
	//右下
	vertexData[3].position = { 1.0f,-1.0f,1.0f };

	//背面
	//左上
	vertexData[4].position = { -1.0f,1.0f,-1.0f };
	//右上
	vertexData[5].position = { 1.0f,1.0f,-1.0f };
	//左下
	vertexData[6].position = { -1.0f,-1.0f,-1.0f };
	//右下
	vertexData[7].position = { 1.0f,-1.0f,-1.0f };


	indexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(uint32_t) * 36);

	//リソースの先頭アドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 36;
	//インデックス
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexDataSprite = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

	//前面
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;

	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;

	//背面
	indexDataSprite[6] = 5;
	indexDataSprite[7] = 4;
	indexDataSprite[8] = 6;

	indexDataSprite[9] = 6;
	indexDataSprite[10] = 7;
	indexDataSprite[11] = 5;

	//右面
	indexDataSprite[12] = 1;
	indexDataSprite[13] = 5;
	indexDataSprite[14] = 3;

	indexDataSprite[15] = 5;
	indexDataSprite[16] = 7;
	indexDataSprite[17] = 3;

	//左面
	indexDataSprite[18] = 4;
	indexDataSprite[19] = 0;
	indexDataSprite[20] = 6;

	indexDataSprite[21] = 0;
	indexDataSprite[22] = 2;
	indexDataSprite[23] = 6;

	//上面
	indexDataSprite[24] = 4;
	indexDataSprite[25] = 5;
	indexDataSprite[26] = 0;

	indexDataSprite[27] = 5;
	indexDataSprite[28] = 1;
	indexDataSprite[29] = 0;

	//下面
	indexDataSprite[30] = 2;
	indexDataSprite[31] = 3;
	indexDataSprite[32] = 6;

	indexDataSprite[33] = 3;
	indexDataSprite[34] = 7;
	indexDataSprite[35] = 6;


	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(SkyBoxMaterial));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	//world用のリソース
	wvpResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(SkyBoxWorldTransformation));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();
}

SkyBoxModel::~SkyBoxModel()
{
	delete pso_;
	pso_ = nullptr;
	vertexResource_->Release();
	indexResource_->Release();
	wvpResource_->Release();
	materialResource_->Release();
}

void SkyBoxModel::Initialize()
{
	world_.Initialize();

}

void SkyBoxModel::Update()
{
	world_.UpdateMatrix();
}

void SkyBoxModel::Draw(const Camera* camera)
{
	pso_->PreDraw();

	Matrix4x4 WVP = world_.matWorld_ * camera->GetViewProjectionMatrix();

	wvpData_->WVP = WVP;

	DXF_->GetCMDList()->IASetVertexBuffers(0,1,&vertexBufferView_);
	DXF_->GetCMDList()->IASetIndexBuffer(&indexBufferView_);
	//wvp
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//マテリアルCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//画像
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, texture_);
	//描画
	DXF_->GetCMDList()->DrawIndexedInstanced(36, 1, 0, 0, 0);
}
