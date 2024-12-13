#pragma once
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<string>

IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfire
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler);

//DXC関係をまとめたクラス
class DXCManager {
public:	//**シングルトンパターン**//
	static DXCManager* GetInstance();
private:
	DXCManager() = default;
	~DXCManager() = default;
	DXCManager(const DXCManager& o) = delete;
	const DXCManager& operator=(const DXCManager& o) = delete;

public:	//**パブリッククラス**//

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

public:	//**ゲッター**//

	IDxcUtils* GetDxcUtils() { return dxcUtils.Get(); }
	IDxcCompiler3* GetDxcCompiler() { return dxcCompiler.Get(); }
	IDxcIncludeHandler* GetIncludeHandler() { return includeHandler.Get(); }

private: //**プライベート変数

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	//dxcUtils
	ComPtr<IDxcUtils> dxcUtils = nullptr;
	//dxcコンパイラ
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	//includeハンドラー
	ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
};