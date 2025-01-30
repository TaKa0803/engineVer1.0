#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

namespace Engine {
	//UAV管理マネージャ
	class UAVManager {

	public://シングルトンパターン
		static UAVManager* GetInstance();
	private:
		UAVManager() = default;
		~UAVManager() = default;
		UAVManager(const UAVManager& o) = delete;
		const UAVManager& operator=(const UAVManager& o) = delete;

	public://**パブリック関数**//

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();

		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// 使っていないものを取得
		/// </summary>
		/// <returns>使っていないハンドル</returns>
		Handles GetDescriptorHandle();

		/// <summary>
		/// ヒープの取得
		/// </summary>
		/// <returns></returns>
		ID3D12DescriptorHeap* GetDescriptorHeap() { return uavDescriptorHeap_; }

	private://**プライベート変数**//

		//DXFポインタ
		Engine::DirectXFunc* DXF_;

		//ディスクリプタヒープ
		ID3D12DescriptorHeap* uavDescriptorHeap_;
		uint32_t descriptorSizeUAV_;

		//UAVの現在の数
		uint32_t UAVNum_ = 0;

		//最大サイズ
		const uint32_t maxUAVSize_ = 256;
	};

}