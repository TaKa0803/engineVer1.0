#pragma once
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"

#include<string>
#include<vector>
#include<array>
#include<DirectXTex/d3dx12.h>
#include<DirectXTex/DirectXTex.h>

#include<wrl.h>
#include<map>

	//画像管理マネージャ
	class TextureManager {
	public://シングルトンパターン
		/// <summary>
		/// インスタンス取得
		/// </summary>
		/// <returns><インスタンス/returns>
		static TextureManager* GetInstance();
	private:
		TextureManager() = default;
		~TextureManager() = default;
		TextureManager(const TextureManager& o) = delete;
		const TextureManager& operator=(const TextureManager& o) = delete;

	public://**パブリック変数

		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		/// <summary>
		/// 仮利用の画像
		/// </summary>
		static int uvChecker_;

		//城画像
		static int white_;

	public://**パブリック関数**//

		/// <summary>
		/// 画像の読み込み処理
		/// </summary>
		/// <param name="filePath"></param>
		/// <returns></returns>
		static int LoadTex(const std::string& filePath);

		/// <summary>
		/// resources/Texture/のショートパスで読み込み
		/// </summary>
		/// <param name="filePath">画像パス</param>
		/// <returns></returns>
		static int LoadTexShortPath(const std::string& filePath);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="texNum">画像番号</param>
		/// <returns>GPUハンドル</returns>
		static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int texNum);

		/// <summary>
		/// 読み込んである画像のGPUハンドルを返す
		/// </summary>
		/// <param name="name">画像のパス</param>
		/// <returns></returns>
		static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(std::string name);


		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="DXF_"></param>
		void Initialize(Engine::DirectXFunc* DXF_);

		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

	private://**プライベート関数**//

		/// <summary>
		/// データを作成
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <param name="mipImages"></param>
		/// <returns></returns>
		int CreateData(const std::string& filePath, const DirectX::ScratchImage& mipImages);

		/// <summary>
		/// 似たようなデータがないかチェック
		/// </summary>
		/// <param name="filepath">パス</param>
		/// <returns>あったか否か</returns>
		bool CheckSameData(const std::string& filepath);

		/// <summary>
		/// パスに合ったテクスチャハンドルへのイテレータ返却
		/// </summary>
		/// <param name="path">パス</param>
		/// <returns>イテレータ番号</returns>
		int GetDataFromPath(const std::string& path);

	private://**プライベート変数**//

		//インスタンス
		Engine::DirectXFunc* DXF = nullptr;

		//簡易パス
		std::string shortPath_ = "resources/Texture/";

		//タグとHandleのデータ群
		std::map<std::string, Handles>tagNumDatas_;

		//番号から参照する用の
		std::map<int, std::string>texDatas_;

		//UVチェッカー画像のパス
		std::string uvCheckerTex_ = "resources/Texture/SystemResources/uvChecker.png";

		//const size_t maxTexNum_ = 256;
	};
