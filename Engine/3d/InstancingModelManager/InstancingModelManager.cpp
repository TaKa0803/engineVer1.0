#include "InstancingModelManager.h"
#include"InstancingGameObject/InstancingModel.h"
#include<cassert>
#include<fstream>
#include<json.hpp>

#include<Windows.h>
#include<winuser.h>


InstancingModelManager* InstancingModelManager::GetInstance() {
	static InstancingModelManager instance;
	return &instance;
}

void InstancingModelManager::Initialize(bool loadAllModel)
{

	pso_ = new InstancingPSO();
	pso_->Initialize();

	if (loadAllModel) {
		LoadAllModel();
	}
}

void InstancingModelManager::LoadAllModel() {

	//読み込むjsonファイル
	std::string filepath = modelPathFile_;
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
	nlohmann::json::iterator itGroup = root.find(groupName_);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

		if (modelNum <= maxModelNum) {
			//アイテムの名前を取得
			const std::string& itemName = itItem.key();

			if (itItem->is_array() && itItem->size() == 3) {
				//モデル群の八田フォルダまでのパス
				std::string foldaPath = itItem->at(0);

				//パスを取得
				std::string modelPath = itItem->at(1);

				//インスタンシング数取得
				int instancingNum = (int)itItem->at(2);


				//モデルデータを作成して設定
				InstancingModel* newModel = new InstancingModel();
				newModel = InstancingModel::CreateFromOBJ(baseFolda_+foldaPath, modelPath, instancingNum);
				newModel->SetTag(itemName);
				//送信
				modelDatas_[itemName] = (newModel);


				//カウンター増加
				modelNum++;
			}
		}
		else {
			//モデル読み込みしすぎ！
			assert(false);
		}
	}


}

void InstancingModelManager::Finalize()
{

	delete pso_;
	pso_ = nullptr;

	for (auto& data : modelDatas_) {
		delete data.second;
		data.second = nullptr;
	}
}

void InstancingModelManager::DrawAllModel() {

	for (auto& model : modelDatas_) {
		if (model.second->GetWorldNum() != 0) {
			model.second->Draw();
		}
	}

}

bool InstancingModelManager::SerchTag(const std::string& tag) {

	//タグをチェック
	if (modelDatas_.find(tag) != modelDatas_.end()) {
		return true;
	}
	else {
		return false;
	}

}

void InstancingModelManager::Debug(const std::string& tag,const char*name)
{
	//データがあるかチェック
	if (modelDatas_.find(tag) != modelDatas_.end()) {
		modelDatas_[tag]->Debug(name);
	}
	else {
		//見つからない
		assert(false);
	}

}

float InstancingModelManager::GetAlpha(const std::string& tag)
{

	if (modelDatas_.find(tag) != modelDatas_.end()) {
		return modelDatas_[tag]->GetColor().w;
	}
	else {
		//データが見つからない
		assert(false);
	}

	return 0;
}

void InstancingModelManager::SetData(const std::string& tag, const EulerWorldTransform& world, int animeNum,const Vector4&color ) {

	if (modelDatas_.find(tag) != modelDatas_.end()) {
		modelDatas_[tag]->AddInstancingData(world,animeNum, color);
	}
	else {
		//タグミスによるエラー処理
		assert(false);
	}

}

void InstancingModelManager::SetTexture(const std::string& tag, int texture) {
	////タグのモデルデータ探索
	//for (auto& model : modeldatas_) {
	//	//タグを発見
	//	if (model->GetTag() == tag) {
	//		//ワールドを追加
	//		model->SetTexture(texture);
	//	}
	//}

	modelDatas_[tag]->SetTexture(texture);

}

void InstancingModelManager::SetBlendMode(const std::string& tag, BlendMode blend) {
	////タグのモデルデータ探索
	//for (auto& model : modeldatas_) {
	//	//タグを発見
	//	if (model->GetTag() == tag) {
	//		//ワールドを追加
	//		model->SetBlendMode(blend);
	//	}
	//}

	modelDatas_[tag]->SetBlendMode(blend);
}

void InstancingModelManager::SetFillMode(const std::string& tag, FillMode fillMode) {
	//タグのモデルデータ探索
	//for (auto& model : modeldatas_) {
	//	//タグを発見
	//	if (model->GetTag() == tag) {
	//		//ワールドを追加
	//		model->SetFillMode(fillMode);
	//	}
	//}

	modelDatas_[tag]->SetFillMode(fillMode);

}

void InstancingModelManager::SetAlpha(const std::string& tag, float alpha) {

	////タグのモデルデータ探索
	//for (auto& model : modeldatas_) {
	//	//タグを発見
	//	if (model->GetTag() == tag) {
	//		//ワールドを追加
	//		model->SetAlpha(alpha);
	//	}
	//}

	modelDatas_[tag]->SetAlpha(alpha);

	//タグ未発見
	//assert(false);
}



void InstancingModelManager::SetEnableTexture(const std::string& tag, bool isEnable) {
	////タグのモデルデータ探索
	//for (auto& model : modeldatas_) {
	//	//タグを発見
	//	if (model->GetTag() == tag) {
	//		//ワールドを追加
	//		model->SetEnableTexture(isEnable);
	//	}
	//}

	modelDatas_[tag]->SetEnableTexture(isEnable);

}

void InstancingModelManager::SetEnableShader(const std::string& tag, bool isEnable)
{
	modelDatas_[tag]->SetEnableShader(isEnable);
}

void InstancingModelManager::SetEnableEnviomentMap(const std::string& tag, bool isEnable, float num)
{
	modelDatas_[tag]->SetEnableEnviomentMap(isEnable, num);
}
