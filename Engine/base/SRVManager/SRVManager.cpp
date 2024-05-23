#include "SRVManager.h"
#include"functions/function.h"
#include<cassert>

SRVManager* SRVManager::GetInstance() {
	static SRVManager instance;
	return &instance;
}

Handles SRVManager::CreateTextureSRV(ID3D12Resource* textureResource, ID3D12Resource* intermediateResource, D3D12_SHADER_RESOURCE_VIEW_DESC& srvdesc)
{
	SRVManager* SRVM = SRVManager::GetInstance();
	
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = SRVM->GetCPU_DES_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = SRVM->GetGPU_DES_HANDLE();

	//リストに登録したリソースのポインタを取得
	ID3D12Resource* listResource = SRVM->PushTextureResource(textureResource);

	//srvの生成
	DirectXFunc::GetInstance()->GetDevice()->CreateShaderResourceView(listResource, &srvdesc, textureSrvHandleCPU);

	textureResource = listResource;

	//GPUに送るように準備
	SRVM->AddIntermediaResource(intermediateResource);
	
	//GPUHandleデータ群に追加して参照できる要素番号取得
	int texnum= SRVM->AddtextureNum(textureSrvHandleGPU);

	//データをまとめる
	Handles data = {
			.cpu{textureSrvHandleCPU},
			.gpu{textureSrvHandleGPU},
			.textureNum{texnum}
	};	
	return data;

}

Handles SRVManager::CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc)
{
	SRVManager* SRVM = SRVManager::GetInstance();

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = SRVM->GetCPU_DES_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = SRVM->GetGPU_DES_HANDLE();

	//srvの生成
	DirectXFunc::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &srvDesc, textureSrvHandleCPU);

	//GPUHandleを登録してサイズを増加
	SRVM->AddtextureNum(textureSrvHandleGPU);

	//返却用構造体
	Handles result = {
		textureSrvHandleCPU,
		textureSrvHandleGPU
	};

	return result;
}

Handles SRVManager::CreateNewSRVHandles()
{
	SRVManager* SRVM = SRVManager::GetInstance();

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = SRVM->GetCPU_DES_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = SRVM->GetGPU_DES_HANDLE();

	//GPUHandleを登録してサイズを増加
	SRVM->AddtextureNum(textureSrvHandleGPU);

	//返却用構造体
	Handles result = {
		textureSrvHandleCPU,
		textureSrvHandleGPU
	};

	return result;
}





void SRVManager::Initialize(ID3D12Device* DXF) {

	DXF_ = DXF;

	//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
	srvDescriptorHeap = CreateDescriptorHeap(DXF_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxSRVSize_, true);
	descriptorSizeSRV = DXF_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//datas_.resize(maxSRVSize_);
}

void SRVManager::PostInitialize() {
	//画像読み込み数がゼロでない場合
	if (intermediaResources_.size() != 0) {
		//開放する
		for (auto& inter : intermediaResources_) {
			if (inter != nullptr) {
				inter->Release();
				inter = nullptr;
			}
		}

		//無いものを消す
		intermediaResources_.remove_if([](auto& data) {
			if (data == nullptr) {
				return true;
			}
			else {
				return false;
			}
			});

	}
}

void SRVManager::Finalize() {
	//yomikomigai開放する
	for (auto& inter : intermediaResources_) {
		if (inter != nullptr) {
			inter->Release();
			inter = nullptr;
		}
	}

	for (ID3D12Resource* texRsource : textureResources_) {
		if (texRsource != nullptr) {
			texRsource->Release();
			texRsource = nullptr;
		}
	}

	srvDescriptorHeap->Release();

}

ID3D12Resource* SRVManager::PushTextureResource(ID3D12Resource* resource) {
	textureResources_.emplace_back(std::move(resource));
	size_t num = textureResources_.size() - 1;
	return textureResources_[num];
}

int SRVManager::AddtextureNum(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU)
{
		//サイズカウントの位置にhandleを設置
		hDatas_[SRVsize_] = textureSrvHandleGPU;
		//GPUと紐付けされた値を保存
		int ans = SRVsize_;

		//値を増やしてずらす
		SRVsize_++;
		//一応SRV大量時にエラー
		if (SRVsize_ >= (int)maxSRVSize_) {
			assert(false);
		}

		return ans;	
}

void SRVManager::AddIntermediaResource(ID3D12Resource* intermediateResource)
{
	intermediaResources_.emplace_back(std::move(intermediateResource));
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPU_DES_HANDLE() {
	return GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, SRVsize_);
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPU_DES_HANDLE() {
	return  GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, SRVsize_);
}
