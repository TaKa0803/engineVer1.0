#pragma once
#include<d3d12.h>
#include<stdint.h>

#include<string>
#include<vector>
#include<map>
#include<dxcapi.h>

#include"struct.h"
#include"Quaternion.h"

//Vector3の遷移処理
Vector3 CalculateValue(const std::vector<KayframeVector3>& keyframes, float time);

//Quaterinionの遷移処理
Quaternion CalculateValue(const std::vector<KayframeQuaternion>& keyframes, float time);

//モデルファイル読み込み
ModelAllData LoadModelFile(const std::string& directoryPath, const std::string& modelName);

//スケルトンの更新
void Update(Skeleton& skeleton);

//スケルトンからスキンクラスターの更新
void Update(SkinCluster& skinCluster, const Skeleton& skeleton);

//スケルトンデータ作成
Skeleton CreateSkeleton(const Node& node);

//s金クラスター作成
SkinCluster CreateSkinCluster(ID3D12Device& device, const Skeleton& skeleton, const ModelData& modelData);

//バッファリソースの作成
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

//ディスクリプタヒープの作成
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

//深度地リソース作成
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

//CPUのディスクリプタを取得
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

//GPUのディスクリプタを取得
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

