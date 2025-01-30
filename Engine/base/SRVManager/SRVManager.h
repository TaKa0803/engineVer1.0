#pragma once
#include<vector>
#include<map>
#include<list>
#include<d3d12.h>
#include"DirectXFunc/DirectXFunc.h"

#include"struct.h"

namespace Engine {
	//SRVの管理マネージャ
	class SRVManager {

	public://シングルトンパターン
		/// <summary>
		/// インスタンス取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static SRVManager* GetInstance();
	private:
		SRVManager() = default;
		~SRVManager() = default;
		SRVManager(const SRVManager& o) = delete;
		const SRVManager& operator=(const SRVManager& o) = delete;

	public://**パブリック関数**//

		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="DXF"></param>
		void Initialize(ID3D12Device* DXF);

		/// <summary>
		/// Texture読み込みがすべて終わった後GPUに送る処理
		/// </summary>
		void PostInitialize();

		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// 画像のSRVの作成
		/// </summary>
		/// <param name="textureResource"></param>
		/// <param name="intermediateResource"></param>
		/// <param name="srvdesc"></param>
		/// <returns>データ型のイテレータ返却</returns>
		static Handles CreateTextureSRV(ID3D12Resource* textureResource, ID3D12Resource* intermediateResource, D3D12_SHADER_RESOURCE_VIEW_DESC& srvdesc);

		/// <summary>
		/// リソースを登録せずSRV作成
		/// </summary>
		/// <param name="resource"></param>
		/// <param name="srvDesc"></param>
		/// <returns></returns>
		static Handles CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

		/// <summary>
		/// 新しいSRVの各Handleを取得して返却する
		/// </summary>
		/// <returns>新しいSRVのHandles</returns>
		static Handles CreateNewSRVHandles();

		//SRVDescriptorHeapを取得
		ID3D12DescriptorHeap* GetSRV()const { return srvDescriptorHeap; }

		/// <summary>
		/// 要素番号のGPUHandleを返却
		/// </summary>
		/// <param name="num"></param>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetTextureDescriptorHandle(int num) {
			return hDatas_[num];
		}

	private://**プライベート関数**//

		/// <summary>
		/// リソースをデータ群に追加してデータ群のポインタ返却
		/// </summary>
		/// <param name="resource"></param>
		/// <returns></returns>
		ID3D12Resource* PushTextureResource(ID3D12Resource* resource);

		/// <summary>
		/// GPUHandleを登録してサイズを増加
		/// </summary>
		/// <param name="textureSrvHandleGPU">GPUHandle</param>
		/// <returns>Handleが登録されたデータ群の要素番号</returns>
		int AddtextureNum(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

		/// <summary>
		/// GPUに送るResourceの設定
		/// </summary>
		/// <param name="intermediateResource"></param>
		void AddIntermediaResource(ID3D12Resource* intermediateResource);

		//使ってない位置のCPUHandleを取得
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPU_DES_HANDLE();
		//使ってない位置のGPUHandleを取得
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_DES_HANDLE();


	private://**プラべーと変数**//

		//DXFのポインタ
		ID3D12Device* DXF_ = nullptr;

		//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
		ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;
		uint32_t descriptorSizeSRV = 0u;

		//リソースデータ群
		std::vector<ID3D12Resource*>textureResources_;

		//GPUに送るデータ群
		std::list<ID3D12Resource*>intermediaResources_;

		//データ群
		//std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> datas_;

		//GPUHandleデータ群
		std::map<int, D3D12_GPU_DESCRIPTOR_HANDLE>hDatas_;

		//SRVの数
		int SRVsize_ = 0;

		//最大SRV量
		const UINT maxSRVSize_ = 256;
	};

}

