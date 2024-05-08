#pragma once
#include<vector>
#include<map>
#include<list>
#include<d3d12.h>
#include"DirectXFunc/DirectXFunc.h"

struct Handles {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu;
};



class SRVManager {

public://シングルトンパターン
	static SRVManager* GetInstance();

	
private:
	SRVManager() = default;
	~SRVManager() = default;
	SRVManager(const SRVManager& o) = delete;
	const SRVManager& operator=(const SRVManager& o) = delete;

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="textureResource"></param>
	/// <param name="intermediateResource"></param>
	/// <param name="srvdesc"></param>
	/// <returns>データ型のイテレータ返却</returns>
	int CreateSRV(ID3D12Resource* textureResource, ID3D12Resource* intermediateResource, D3D12_SHADER_RESOURCE_VIEW_DESC& srvdesc);

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="textureResource"></param>
	/// <param name="srvdesc"></param>
	/// <returns>GPUHandleの返却</returns>
	Handles CreateSRV(ID3D12Resource* textureResource, D3D12_SHADER_RESOURCE_VIEW_DESC& srvdesc);

	Handles CreateNewSRVHandles();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="DXF"></param>
	void Initialize(ID3D12Device*DXF);

	/// <summary>
	/// Texture読み込みがすべて終わった後GPUに送る処理
	/// </summary>
	void PostInitialize();

	void Finalize();

	ID3D12Resource* PushTextureResource(ID3D12Resource* resource);

	/// <summary>
	/// GPUHandleを登録してサイズを増加
	/// </summary>
	/// <param name="textureSrvHandleGPU"></param>
	/// <returns></returns>
	int AddtextureNum(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) {
		//サイズカウントの位置にhandleを設置
		hDatas_[SRVsize_] = textureSrvHandleGPU;
		//GPUと紐付けされた値を保存
		int ans = SRVsize_;
		
		//値を増やしてずらす
		SRVsize_++;

		return ans;
	}
public:

	ID3D12DescriptorHeap* GetSRV()const { return srvDescriptorHeap; }

	uint32_t GetSRVSize()const { return descriptorSizeSRV; }

	int GetGPUHandleDataSize() { return (int)hDatas_.size(); }

	//使ってないサイズを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPU_DES_HANDLE();
	//使ってないサイズを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_DES_HANDLE();

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureDescriptorHandle(int num) {
		return hDatas_[num];
	}
private:

	
	ID3D12Device* DXF_=nullptr;

	//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
	ID3D12DescriptorHeap* srvDescriptorHeap=nullptr;
	uint32_t descriptorSizeSRV=0u;

	std::vector<ID3D12Resource*>textureResources_;

	std::list<ID3D12Resource*>intermediaResources_;

	//データ群
	//std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> datas_;
	std::map<int, D3D12_GPU_DESCRIPTOR_HANDLE>hDatas_;
	
	//SRV値初期値
	int SRVsize_ = 0;

	const size_t maxSRVSize_ = 256;
};

