#pragma once
#include"struct.h"
#include<vector>
#include<string>
#include<iostream>

#include"WorldTransform/WorldTransform.h"
#include"DirectXFunc/DirectXFunc.h"
#include"SingleGameObjects/ObjectPSO.h"
#include"struct.h"


class ModelManager {

public://シングルトンパターン
	static ModelManager* GetInstance();

private://シングルトンパターン

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager& o) = delete;
	const ModelManager& operator=(const ModelManager& o) = delete;


public:	

	void SepUp();

	void LoadAllModels();

	void Finalize();

	/// <summary>
	/// モデルデータの取得
	/// </summary>
	/// <param name="filename">modelの名前またはresources以降のパス</param>
	/// <returns>モデルデータ</returns>
	ModelData GetModelData( const std::string& filename);

	//描画前処理
	static void PreDraw(FillMode fillMode = FillMode::kSolid, BlendMode blendMode = BlendMode::kNormal);

private:

	//各モデル描画用のPSO
	ObjectPSO* grarphics_;

	//モデルデータのパスが記入されたファイルへのパス
	const std::string modelPathFile = "resources/jsonfile/modelPathFile.json";
	
	//モデルデータを入れるフォルダ
	const std::string modeldirectoryPath = "resources/Models/";

	//モデルデータのパス群
	const std::string groupName = "modelPathFile";



	//データ保存の構造体
	struct NameAndPath {
		std::string name;
		std::string path;
	};

	//保存データ構造
	std::vector<std::pair<NameAndPath, ModelData>> modelDatas;

	//最大量設定
	const size_t maxModelData = 256;

	//GetModelDataでモデルが見つからない場合エラーをはくか否か
	bool isError = true;


};

