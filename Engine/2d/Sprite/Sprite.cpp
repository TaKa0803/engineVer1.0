#include"Sprite.h"
#include"Math/Matrix.h"
#include<cassert>
#include"TextureManager/TextureManager.h"
#include"SRVManager/SRVManager.h"
#include"functions/function.h"
#include<imgui.h>
#include"Log/Log.h"

#include"SpriteManager/SpriteManager.h"

using namespace Engine;

Sprite::Sprite() {
}

Sprite::~Sprite() {
	
	//解放処理
	vertexResource_->Release();
	indexResource_->Release();
	transformationMatrixResource_->Release();
	materialResource_->Release();
}

Sprite* Sprite::Create(int texture, const Vector2 size, const Vector2 Rect, const Vector2 scale , const Vector2 translate, const Vector2 anchor, const float rotate) {
	//DXFのインスタンス取得
	Engine::DirectXFunc* DXF = Engine::DirectXFunc::GetInstance();
	//ワールド作成
	EulerWorldTransform newWorld;
	newWorld.translate_ = { translate.x,translate.y,0 };
	newWorld.rotate_.z = rotate;
	newWorld.scale_ = { scale.x,scale.y,1 };
#pragma region Sprite
	//リソース作成
	ID3D12Resource* vertexResource;
	ID3D12Resource* indexResourceSprite;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

#pragma region VertexResourceとVertexBufferViewを用意
	//Sprite用の頂点リソースを作る
	vertexResource = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * 4);
#pragma endregion
#pragma region 頂点データを設定する
	VertexData* vertexDataSprite = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//一枚目の三角形
	Vector2 minv = { 1 * (-anchor.x),1 * (-anchor.y) };
	Vector2 maxV = { 1 * (1 - anchor.x),1 * (1 - anchor.y) };
	Vector2 maxUV = { Rect.x / size.x,Rect.y/size.y };

	vertexDataSprite[0].position = { minv.x,maxV.y,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,maxUV.y };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[1].position = { minv.x,minv.y,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[2].position = { maxV.x,maxV.y,0.0f,1.0f };
	vertexDataSprite[2].texcoord = maxUV;
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[3].position = { maxV.x,minv.y,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { maxUV.x,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	indexResourceSprite = CreateBufferResource(DXF->GetDevice(), sizeof(uint32_t) * 6);

	//リソースの先頭アドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックス
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	//インデックス作成
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 1;
	indexDataSprite[1] = 3;
	indexDataSprite[2] = 0;

	indexDataSprite[3] = 3;
	indexDataSprite[4] = 2;
	indexDataSprite[5] = 0;

#pragma endregion

	//生成して初期化
	Sprite* sprite = new Sprite();
	sprite->Initialize(texture,newWorld, vertexResource, indexResourceSprite, indexBufferViewSprite);

	//ログ出力
	Log("Sprite is Created!\n");

	return sprite;
}

GlobalVariableTree& Sprite::GetTree(const std::string& name)
{
	
	//デバッグ用のツリーにデータを登録
	tree_.name_ = name;
	tree_.SetTreeData(world_.GetDebugTree());
	tree_.SetTreeData(uvWorld_.GetDebugTree("UVワールド"));
	tree_.SetValue("画像", &materialData_->enableTexture);
	tree_.SetValue("色", &materialData_->color);
	tree_.SetValue("棄却量", &materialData_->discardNum);
	return tree_;
}

void Sprite::Initialize(int texture,

	EulerWorldTransform world,

	ID3D12Resource* vertexResource,

	ID3D12Resource* indexResourceSprite,

	D3D12_INDEX_BUFFER_VIEW indexBufferView
) {
	DXF_ = Engine::DirectXFunc::GetInstance();

	//作成したデータコピー
	texture_ = texture;
	world_ = world;
	vertexResource_ = vertexResource;
	indexResource_ = indexResourceSprite;
	indexBufferView_ = indexBufferView;

	//頂点バッファビューを作成する
	//リソース用の先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Transform周りを作る
	transformationMatrixResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む

	//書き込むためのアドレスを取得
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();


	//Sprite用のマテリアルリソース
	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(Sprite));
	//マテリアルにデータを書き込む
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);	
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableTexture = true;

}


void Sprite::Draw(int texture) {

	//PSO
	//仮で変更、後で修正するように
	SpriteManager::PreDraw();

	//uv更新
	uvWorld_.UpdateMatrix();
	//uvTransform更新
	materialData_->uvTransform = uvWorld_.matWorld_;

	//ワールド更新
	world_.UpdateMatrix();
	Matrix4x4 World = world_.matWorld_;

	//スプライト用データ
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowApp::kClientWidth), float(WindowApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 VPSprite = viewMatrix * projectionMatrixSprite;
	Matrix4x4 WVP = World * VPSprite;
	//データ代入
	transformationMatrixData_->WVP = WVP;
	transformationMatrixData_->World = World;

	//Spriteの描画
	DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定			
	DXF_->GetCMDList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	//
	if (texture != -1) {
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
	}
	else {
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture_));
	}
	//描画！！（DrawCall
	DXF_->GetCMDList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::Draw(D3D12_GPU_DESCRIPTOR_HANDLE tex)
{
	//PSO
//仮で変更、後で修正するように
	SpriteManager::PreDraw();

	//uv更新
	uvWorld_.UpdateMatrix();
	//uvTransform更新
	materialData_->uvTransform = uvWorld_.matWorld_;

	//ワールド更新
	world_.UpdateMatrix();
	Matrix4x4 World = world_.matWorld_;

	//スプライト用データ
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowApp::kClientWidth), float(WindowApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 VPSprite = viewMatrix * projectionMatrixSprite;
	Matrix4x4 WVP = World * VPSprite;
	//データ代入
	transformationMatrixData_->WVP = WVP;
	transformationMatrixData_->World = World;

	//Spriteの描画
	DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定			
	DXF_->GetCMDList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	//画像設定
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, tex);
	//描画！！（DrawCall
	DXF_->GetCMDList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}



