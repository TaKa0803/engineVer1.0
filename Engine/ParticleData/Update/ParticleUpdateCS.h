#pragma once
#include"Vector4.h"
#include"Matrix.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleData/ParticleData.h"

//ComputeShaderで処理するパーティクルの更新処理クラス
class ParticleUpdateCS {
public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleUpdateCS();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleUpdateCS();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="adress"></param>
	/// <param name="chandle"></param>
	/// <param name="listhandle"></param>
	void UpdateGPU(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_VIRTUAL_ADDRESS adress, D3D12_GPU_DESCRIPTOR_HANDLE chandle, D3D12_GPU_DESCRIPTOR_HANDLE listhandle);

private://**プライベート変数**//

	//DXFのポインタ
	DirectXFunc* DXF_ = nullptr;

	//CSのhlslまでのポインタ
	std::wstring csPass = L"resources/shaders/ComputeShader/Particle.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//パーティクル最大数
	size_t maxDataCount_ = 1024;
};