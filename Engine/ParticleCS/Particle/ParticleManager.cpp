#include "ParticleManager.h"
#include"DirectXFunc/DirectXFunc.h"
#include"SingleModelManager/ModelManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"UAVManager/UAVManager.h"
#include"Camera/Camera.h"
#include<numbers>

ParticleManager::ParticleManager()
{
	DXF_ = DirectXFunc::GetInstance();

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

	DXF_->GetDevice()->CreateUnorderedAccessView(particleResource_, nullptr, &oUAVDesc, UAVHandle_.cpu);

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

	SRVHandle_ = SRVManager::CreateSRV(particleResource_, instancingDesc);
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

	DXF_->GetDevice()->CreateUnorderedAccessView(freeListResource_, nullptr, &fUAVDesc, listUAVHandle_.cpu);

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

	DXF_->GetDevice()->CreateUnorderedAccessView(freeListIndexResource_, nullptr, &cfUAVDesc, counterUAVHandle_.cpu);

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

	counterSRVHandle_ = SRVManager::CreateSRV(freeListIndexResource_, instancingDesccount);
#pragma endregion



	perResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(PerView));
	perResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
	perViewData_->viewProjection = MakeIdentity4x4();
	perViewData_->billboardMatrix = MakeIdentity4x4();

	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(ParticleMaterialData));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1,1,1,1 };
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableTex = true;
	materialData_->discardNum = 0.0f;




	pso_ = std::make_unique<ParticlePSO>();
	pso_->Initialize();

	particleInitializeCS_ = std::make_unique<ParticleInitializeCS>();

	emiterCS_ = std::make_unique<ParticleEmiterCS>();

	particleUpdateCS_ = std::make_unique<ParticleCS>();


}

ParticleManager::~ParticleManager()
{
	vertexResource_->Release();
	indexResource_->Release();
	particleResource_->Release();
	perResource_->Release();
	materialResource_->Release();
	freeListResource_->Release();
	freeListIndexResource_->Release();

}

void ParticleManager::Initialize(int tex)
{
	//画像取得
	texture_ = SRVManager::GetInstance()->GetTextureDescriptorHandle(tex);
	particleInitializeCS_->Initialize(UAVHandle_.gpu, counterUAVHandle_.gpu, listUAVHandle_.gpu);

	emiterCS_->Initialize();
	emiterCS_->SetOnlyImpact(true);
}

void ParticleManager::Update()
{
#pragma region PerView

	Matrix4x4 backToFrontM = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billM = backToFrontM * Camera::GetInstance()->GetMainCamera().matWorld_;
	billM.m[3][0] = 0.0f;
	billM.m[3][1] = 0.0f;
	billM.m[3][2] = 0.0f;

	perViewData_->viewProjection = Camera::GetInstance()->GetViewProjectionMatrix();
	perViewData_->billboardMatrix = billM;
	perViewData_->deltaTime = 1.0f / 60.0f;
#pragma endregion

	emiterCS_->Update();


}

void ParticleManager::Draw()
{



#pragma region 各データいれる



#pragma endregion
	

	emiterCS_->PreDraw(UAVHandle_.gpu, counterUAVHandle_.gpu, listUAVHandle_.gpu);
	
	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();
	D3D12_RESOURCE_BARRIER ubarrier = {};
	ubarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	ubarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ubarrier.UAV.pResource = particleResource_;
	cmd->ResourceBarrier(1, &ubarrier);

	particleUpdateCS_->PreDraw(UAVHandle_.gpu, perResource_->GetGPUVirtualAddress(), counterUAVHandle_.gpu, listUAVHandle_.gpu);

	ubarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	ubarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ubarrier.UAV.pResource = particleResource_;
	cmd->ResourceBarrier(1, &ubarrier);

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = particleResource_;  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);


	pso_->PreDraw(BlendMode::kAdd);


	ID3D12DescriptorHeap* des[] = { SRVManager::GetInstance()->GetSRV() };
	cmd->SetDescriptorHeaps(1, des);

	cmd->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cmd->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	cmd->SetGraphicsRootDescriptorTable(0, SRVHandle_.gpu);
	cmd->SetGraphicsRootConstantBufferView(1, perResource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());

	cmd->SetGraphicsRootDescriptorTable(3, texture_);

	cmd->DrawIndexedInstanced(static_cast<UINT>(modelData_.indices.size()), 1024, 0, 0, 0);

	// リソースバリアの設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = particleResource_;  // 頂点バッファリソースへのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// コマンドリストにバリアを設定
	cmd->ResourceBarrier(1, &barrier);

}

void ParticleManager::SpawnE(const Vector3& pos)
{
	emiterCS_->Emit(pos);
}
