#include "ParticleManager.h"
#include"DirectXFunc/DirectXFunc.h"
#include"SingleModelManager/ModelManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"UAVManager/UAVManager.h"
#include"Camera/Camera.h"
#include"ImGuiManager/ImGuiManager.h"
#include"DeltaTimer/DeltaTimer.h"
#include<numbers>

ParticleManager::ParticleManager()
{
	//DXFのポインタ取得
	DXF_ = DirectXFunc::GetInstance();

	//モデルデータ読み込み
	modelData_ = ModelManager::GetInstance()->GetModelData("z+Plane").model;

	//頂点データ
	VertexData* vertexData_;

	//頂点リソース
	vertexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//インデックス
	indexResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * (uint32_t)modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	uint32_t* mappedIndex;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());

	//パーティクルの数
	const int particleNum = 1024;

	particleResource_ = CreateUAVBufferResource(DXF_->GetDevice(), sizeof(Particle) * particleNum);

#pragma region UAVのHandle生成
	UAVHandle_ = UAVManager::GetInstance()->GetDescriptorHandle();

	D3D12_UNORDERED_ACCESS_VIEW_DESC oUAVDesc{};
	oUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	oUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	oUAVDesc.Buffer.FirstElement = 0;
	oUAVDesc.Buffer.NumElements = UINT(maxDataNum_);
	oUAVDesc.Buffer.CounterOffsetInBytes = 0;
	oUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	oUAVDesc.Buffer.StructureByteStride = sizeof(Particle);

	DXF_->GetDevice()->CreateUnorderedAccessView(particleResource_.Get(), nullptr, &oUAVDesc, UAVHandle_.cpu);

#pragma endregion


#pragma region SRVのHandle生成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
	instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc.Buffer.FirstElement = 0;
	instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc.Buffer.NumElements = UINT(maxDataNum_);
	instancingDesc.Buffer.StructureByteStride = sizeof(Particle);

	SRVHandle_ = SRVManager::CreateSRV(particleResource_.Get(), instancingDesc);
#pragma endregion


	freeListResource_ = CreateUAVBufferResource(DXF_->GetDevice(), sizeof(uint32_t) * maxDataNum_);

#pragma region UAVのHandle生成
	listUAVHandle_ = UAVManager::GetInstance()->GetDescriptorHandle();

	D3D12_UNORDERED_ACCESS_VIEW_DESC fUAVDesc{};
	fUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	fUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	fUAVDesc.Buffer.FirstElement = 0;
	fUAVDesc.Buffer.NumElements = UINT(maxDataNum_);
	fUAVDesc.Buffer.CounterOffsetInBytes = 0;
	fUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	fUAVDesc.Buffer.StructureByteStride = sizeof(int32_t);

	DXF_->GetDevice()->CreateUnorderedAccessView(freeListResource_.Get(), nullptr, &fUAVDesc, listUAVHandle_.cpu);

#pragma endregion

	freeListIndexResource_ = CreateUAVBufferResource(DXF_->GetDevice(), sizeof(int32_t) * 1);

#pragma region UAVのHandle生成
	counterUAVHandle_ = UAVManager::GetInstance()->GetDescriptorHandle();

	D3D12_UNORDERED_ACCESS_VIEW_DESC cfUAVDesc{};
	cfUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	cfUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	cfUAVDesc.Buffer.FirstElement = 0;
	cfUAVDesc.Buffer.NumElements = UINT(1);
	cfUAVDesc.Buffer.CounterOffsetInBytes = 0;
	cfUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	cfUAVDesc.Buffer.StructureByteStride = sizeof(int32_t);

	DXF_->GetDevice()->CreateUnorderedAccessView(freeListIndexResource_.Get(), nullptr, &cfUAVDesc, counterUAVHandle_.cpu);

#pragma endregion


#pragma region SRVのHandle生成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesccount{};
	instancingDesccount.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesccount.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesccount.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesccount.Buffer.FirstElement = 0;
	instancingDesccount.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesccount.Buffer.NumElements = UINT(1);
	instancingDesccount.Buffer.StructureByteStride = sizeof(int32_t);

	counterSRVHandle_ = SRVManager::CreateSRV(freeListIndexResource_.Get(), instancingDesccount);
#pragma endregion

	//perリソースの生成
	perResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(PerView));
	perResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
	perViewData_->viewProjection = MakeIdentity4x4();
	perViewData_->billboardMatrix = MakeIdentity4x4();

	//マテリアルリソースの生成
	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(ParticleMaterialData));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1,1,1,1 };
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableTex = true;
	materialData_->discardNum = 0.0f;

	//エミッターリソースの生成
	emiterResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(EmiterSphere));
	emiterResource_->Map(0, nullptr, reinterpret_cast<void**>(&emiterData_));
	//仮でデータ追加
	emiterData_->radius = { 0.0f,1.0f };
	emiterData_->count = {10 ,10 };
	emiterData_->speed = { 0.0f,1.0f };
	emiterData_->veloX = { -1.0f,1.0f };
	emiterData_->veloY = { -1.0f,1.0f };
	emiterData_->veloZ = { -1.0f,1.0f };
	emiterData_->frequency = 0.5f;
	emiterData_->frequencyTime = 0.0f;
	emiterData_->translate = Vector3(0.0f, 0.0f, 0.0f);
	emiterData_->color = { 1,1,1,1 };
	emiterData_->emit = 0;
	emiterData_->lifetime = 1;

	//パイプラインクラス生成と初期化
	pso_ = std::make_unique<ParticlePSO>();
	pso_->Initialize();

	//各クラス生成
	particleInitializeCS_ = std::make_unique<ParticleInitializeCS>();
	emiterCS_ = std::make_unique<ParticleEmiterCS>();
	particleUpdateCS_ = std::make_unique<ParticleUpdateCS>();
}

ParticleManager::~ParticleManager()
{
	//各リソース解放
	vertexResource_->Release();
	indexResource_->Release();
	particleResource_->Release();
	perResource_->Release();
	materialResource_->Release();
	emiterResource_->Release();
	freeListResource_->Release();
	freeListIndexResource_->Release();
}

void ParticleManager::Initialize(int tex)
{
	//画像取得
	texture_ = SRVManager::GetInstance()->GetTextureDescriptorHandle(tex);
	//初期化クラス処理
	particleInitializeCS_->Initialize(UAVHandle_.gpu, counterUAVHandle_.gpu, listUAVHandle_.gpu);
	//エミッター初期化
	emiterCS_->Initialize(emiterResource_->GetGPUVirtualAddress(),emiterData_);
}

void ParticleManager::Update()
{
#pragma region PerView

	Matrix4x4 backToFrontM = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billM = backToFrontM * Camera::GetInstance()->GetMainCamera().matWorld_;
	billM.m[3][0] = 0.0f;
	billM.m[3][1] = 0.0f;
	billM.m[3][2] = 0.0f;

	//データに値追加
	perViewData_->viewProjection = Camera::GetInstance()->GetViewProjectionMatrix();
	perViewData_->billboardMatrix = billM;
	perViewData_->deltaTime = (float)DeltaTimer::deltaTime_;
#pragma endregion

	//エミッターの更新
	emiterCS_->Update(onlyImpact_);
	
	//発生処理
	emiterCS_->EmitGPU(UAVHandle_.gpu, counterUAVHandle_.gpu, listUAVHandle_.gpu);

	//処理を並列に行わせないための処理
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();
	D3D12_RESOURCE_BARRIER ubarrier = {};
	ubarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	ubarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ubarrier.UAV.pResource = particleResource_.Get();
	cmd->ResourceBarrier(1, &ubarrier);

	//パーティクルの更新
	particleUpdateCS_->UpdateGPU(UAVHandle_.gpu, perResource_->GetGPUVirtualAddress(), counterUAVHandle_.gpu, listUAVHandle_.gpu);
}

void ParticleManager::Draw()
{
	//コマンド取得
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();
	
	//バリア生成
	D3D12_RESOURCE_BARRIER ubarrier = {};
	ubarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	ubarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ubarrier.UAV.pResource = particleResource_.Get();
	cmd->ResourceBarrier(1, &ubarrier);

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = particleResource_.Get();  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);

	//描画前処理
	pso_->PreDraw(BlendMode::kAdd);
	
	//ヒープ取得
	ID3D12DescriptorHeap* des[] = { SRVManager::GetInstance()->GetSRV() };
	cmd->SetDescriptorHeaps(1, des);
	//頂点セット
	cmd->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cmd->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//各値セット
	cmd->SetGraphicsRootDescriptorTable(0, SRVHandle_.gpu);
	cmd->SetGraphicsRootConstantBufferView(1, perResource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());
	//画像セット
	cmd->SetGraphicsRootDescriptorTable(3, texture_);
	//パーティクル描画処理
	cmd->DrawIndexedInstanced(static_cast<UINT>(modelData_.indices.size()), 1024, 0, 0, 0);

	// リソースバリアの設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = particleResource_.Get();  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);

}

void ParticleManager::Debug(const std::string name)
{
#ifdef _DEBUG
	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("座標", &emiterData_->translate.x, 0.01f);
	ImGui::DragFloat2("最小/最大:出現半径", &emiterData_->radius.x, 0.01f);
	ImGui::DragFloat2("最小/最大:一度の生成量", &emiterData_->count.x,0.01f);

	ImGui::DragFloat2("最小/最大:速度", &emiterData_->speed.x, 0.01f);
	ImGui::DragFloat2("最小/最大:X軸の飛翔方向", &emiterData_->veloX.x, 0.01f);
	ImGui::DragFloat2("最小/最大:Y軸の飛翔方向", &emiterData_->veloY.x, 0.01f);
	ImGui::DragFloat2("最小/最大:Z軸の飛翔方向", &emiterData_->veloZ.x, 0.01f);

	ImGui::Text("Flag %d", emiterData_->emit);

	ImGui::Checkbox("onlyInput", &onlyImpact_);

	ImGui::DragFloat("生成間隔(秒)", &emiterData_->frequency, 0.01f);

	ImGui::End();

#endif // _DEBUG

}

void ParticleManager::SpawnE(const Vector3& pos)
{
	//出現座標セット
	emiterData_->translate = pos;
	//出現フラグON
	emiterData_->emit = 1;
}

GlobalVariableTree& ParticleManager::GetTreeData(const std::string&name)
{
	//ツリー名セット
	tree_.name_ = name;

	//デバッグ用に値をツリーにセット
	tree_.SetValue("色", &materialData_->color);
	tree_.SetValue("画像の有効化", &materialData_->enableTex);
	tree_.SetValue("関数呼び出し時のみ出力", &onlyImpact_);

	GlobalVariableTree emiter = GlobalVariableTree("エミッター");
	emiter.SetValue("生成間隔/Sec", &emiterData_->frequency);
	emiter.SetValue("最小/最大：出現半径", &emiterData_->radius);
	emiter.SetValue("最小/最大：一度の生成量", &emiterData_->count);
	emiter.SetValue("最小/最大：速度", &emiterData_->speed);
	emiter.SetValue("最小/最大：X軸の飛翔方向", &emiterData_->veloX);
	emiter.SetValue("最小/最大：Y軸の飛翔方向", &emiterData_->veloY);
	emiter.SetValue("最小/最大：Z軸の飛翔方向", &emiterData_->veloZ);
	tree_.SetTreeData(emiter);

	return tree_;
}
