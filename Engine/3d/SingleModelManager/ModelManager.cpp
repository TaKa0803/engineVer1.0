#include "ModelManager.h"

#include<cassert>
#include<fstream>
#include<json.hpp>

#include<Windows.h>
#include<winuser.h>

#include"functions/function.h"
#include"TextureManager/TextureManager.h"
#include"SRVManager/SRVManager.h"


#pragma region ModelManager

ModelManager* ModelManager::GetInstance() {
	//インスタンス生成
	static ModelManager instance;
	return &instance;
}

void ModelManager::Initialize()
{
	//PSOの初期化
	grarphics_ = new ObjectPSO();
	grarphics_->Initialize();
}

void ModelManager::LoadAllModels() {
	//サイズを設定
	modelDatas.resize(maxModelData);

	//読み込むjsonファイル
	std::string filepath = modelPathFile;
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filepath);

	//ファイルオープン失敗処理
	if (ifs.fail()) {
		std::string message = "モデルパス記載のjsonファイルが見つかりません！";
		MessageBoxA(nullptr, message.c_str(), "ModelManager", 0);
		assert(message.c_str());
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//移したのでファイルを閉じる
	ifs.close();

	//グループ検索
	nlohmann::json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテムの名前を取得
		const std::string& itemTag = itItem.key();

		if (itItem->is_array() && itItem->size() == 2) {
			//モデル群の八田フォルダまでのパス
			std::string foldaPath = itItem->at(0);

			//パスを取得
			std::string modelfullPath = itItem->at(1);


			//名前と古パスパスを合わせた構造体
			NameAndPath nameAndPath = { itemTag,modeldirectoryPath + foldaPath+modelfullPath };

			//モデルデータを作成して設定
			ModelAllData newmodelData = LoadModelFile(modeldirectoryPath+foldaPath, modelfullPath);
			std::pair<NameAndPath, ModelAllData>newData(nameAndPath, newmodelData);
			modelDatas.emplace_back(newData);

		}
	}
}

void ModelManager::Finalize()
{
	//PSO削除
	delete grarphics_;
	grarphics_ = nullptr;
}

ModelAllData ModelManager::GetModelData(const std::string& filename) {

	//データ型に該当するものを追加
	for (auto& modeldata : modelDatas) {
		//名前タグが同じorフルパスが同じパスが同じ場合
		if (modeldata.first.name == filename || modeldata.first.path == filename) {
			return modeldata.second;
		}
	}

	//存在しない場合の処理
	if (isAnotherModelLoad_) {
		//モデルデータを作成して設定
		ModelAllData newmodelData = LoadModelFile("resources", filename);
		std::pair<std::string, ModelAllData>newData(filename, newmodelData);
		modelDatas.emplace_back(newData);
		return modelDatas.back().second;	
	}
	else {
		//存在していないのでエラー
		assert(false);

		return ModelAllData();
	}

	
}

void ModelManager::PreDraw( BlendMode blendMode,FillMode fillMode)
{
	//モデルの描画前処理
	ModelManager::GetInstance()->grarphics_->PreDraw(fillMode, blendMode);
}
#pragma endregion


