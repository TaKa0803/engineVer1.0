#pragma once
#include"Vector3.h"
#include"DirectXFunc/DirectXFunc.h"
#include"ParticleData/ParticleData.h"
#include<cstdint>

//パーティクルのエミッター処理
class ParticleEmiterCS {
public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmiterCS();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleEmiterCS();
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="emiterDataAddress"></param>
	/// <param name="emiterData"></param>
	void Initialize(D3D12_GPU_VIRTUAL_ADDRESS emiterDataAddress,EmiterSphere* emiterData);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="onlyImpact">カウント停止フラグ</param>
	void Update(bool onlyImpact);
	
	/// <summary>
	/// CSの処理実行
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="chandle"></param>
	/// <param name="listhand"></param>
	void EmitGPU(D3D12_GPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE chandle, D3D12_GPU_DESCRIPTOR_HANDLE listhand);

	/// <summary>
	/// エミッターデータ取得
	/// </summary>
	/// <returns>データ</returns>
	EmiterSphere* GetEmiterData() { return emiterData_; }

private://**プライベート変数**//

	//DXFのポインタ
	Engine::DirectXFunc* DXF_ = nullptr;
	
	//CSのパス
	std::wstring csPass = L"resources/shaders/ComputeShader/ParticleEmiter.CS.hlsl";

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_ = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	//パーティクル最大数
	size_t maxDataCount_ = 1024;

	//エミッターデータ関連
	D3D12_GPU_VIRTUAL_ADDRESS emiterDataAddress_;
	EmiterSphere* emiterData_ = nullptr;

	//リソース
	ID3D12Resource* perFrameResource_;
	PerFrame* perFrameData_ = nullptr;

};