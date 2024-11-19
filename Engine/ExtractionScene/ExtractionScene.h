#pragma once
#include"Vector4.h"
#include"DirectXFunc/DirectXFunc.h"
#include"PostEffect/PEs/PEOffScreen.h"

class ExtractionScene {

public:

	static ExtractionScene* GetInstance();

private:

	ExtractionScene() = default;
	~ExtractionScene() = default;
	ExtractionScene(const ExtractionScene& o) = delete;
	const ExtractionScene& operator=(const ExtractionScene& o) = delete;

	const Vector4 kRenderTClearValue{ 0.0f,0.0f,0.0f,0.0f };
public:

	void Initialize();

	void Finalize();

	void LoadSceneTexture(ID3D12Resource* res, D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE gHandle,D3D12_CPU_DESCRIPTOR_HANDLE dsv);

	//画像のGPUhandle
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexHandle() { return gHandle_; }
private:

	DirectXFunc* DXF_;

	std::unique_ptr<PEOffScreen>peOffScreen_ = nullptr;

	ID3D12Resource* renderTexture_;

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE cHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle_;


};