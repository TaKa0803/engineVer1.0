#include "ParticleManager.h"
#include"DirectXFunc/DirectXFunc.h"
#include"SingleModelManager/ModelManager.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"

ParticleManager::ParticleManager()
{
	DXF_ = DirectXFunc::GetInstance();

	modelData_=ModelManager::GetInstance()->GetModelData("plane").model;

	//頂点データ
	
	VertexData* vertexData_;


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
	wvpResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(Particle) * particleNum);
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(particleData_));
	
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
	instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc.Buffer.FirstElement = 0;
	instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc.Buffer.NumElements = particleNum;
	instancingDesc.Buffer.StructureByteStride = sizeof(Particle);

	wvpHandles_ = SRVManager::CreateSRV(wvpResource_, instancingDesc).gpu;
	
	perResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(PerView));
	perResource_->Map(0, nullptr, reinterpret_cast<void**>(perViewData_));

	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(MaterialData));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(materialData_));
	materialData_->color = { 1,1,1,1 };
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableTex = true;
	materialData_->discardNum = 0.0f;


}

void ParticleManager::Initialize(int tex)
{
}

void ParticleManager::Draw()
{

	pso_->PreDraw();

	ID3D12GraphicsCommandList* cmd = DXF_->GetCMDList();

	cmd->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cmd->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	cmd->SetGraphicsRootConstantBufferView(1, perResource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());

	

}
