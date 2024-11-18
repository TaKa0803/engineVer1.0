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

class DXCManager {
public://シングルトンパターン
	static DXCManager* GetInstance();
private:
	DXCManager() = default;
	~DXCManager() = default;
	DXCManager(const DXCManager& o) = delete;
	const DXCManager& operator=(const DXCManager& o) = delete;

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	void Initialize();

	void Release();

	IDxcUtils* GetDxcUtils() { return dxcUtils.Get(); }
	IDxcCompiler3* GetDxcCompiler() { return dxcCompiler.Get(); }
	IDxcIncludeHandler* GetIncludeHandler() { return includeHandler.Get(); }
private:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDxcUtils> dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
};