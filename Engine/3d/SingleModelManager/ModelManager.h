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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// パスのモデルをすべて読み込む処理
	/// </summary>
	void LoadAllModels();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// モデルデータの取得
	/// </summary>
	/// <param name="filename">modelの名前またはresources以降のパス</param>
	/// <returns>モデルデータ</returns>
	ModelAllData GetModelData( const std::string& filename);

	/// <summary>
	///各モデル描画前処理
	/// </summary>
	/// <param name="isHaveAnimation">animationがあるか否か</param>
	/// <param name="blendMode">ブレンド設定</param>
	/// <param name="fillMode">埋めるか否か</param>
	static void PreDraw(BlendMode blendMode = BlendMode::kNormal, FillMode fillMode = FillMode::kSolid );
private:

	//各モデル描画用のPSO
	ObjectPSO* grarphics_ = nullptr;


	//モデルデータのパスが記入されたファイルへのパス
	const std::string modelPathFile = "resources/jsonfile/modelPathFile.json";
	
	//モデルデータを入れるフォルダ
	const std::string modeldirectoryPath = "resources/Models/";

	//モデルデータのグループ名
	const std::string groupName = "modelPathFile";

	//データ保存の構造体
	struct NameAndPath {
		std::string name;
		std::string path;
	};

	//保存データ群
	std::vector<std::pair<NameAndPath, ModelAllData>> modelDatas;

	//モデル最大量設定
	const size_t maxModelData = 256;

	//パスに含まれないモデルを読み込まないか否か
	bool isAnotherModelLoad_ = false;


};

