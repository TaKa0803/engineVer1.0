#include"EnemyPopManager.h"

#include<fstream>
#include<cassert>
#include<Windows.h>
#include<winuser.h>
#include<json.hpp>

#include"RandomNum/RandomNum.h"
#include"TextureManager/TextureManager.h"


EnemyPopManager::EnemyPopManager()
{
}

EnemyPopManager::~EnemyPopManager()
{
	popDatas_.clear();
	flagWorlds_.clear();
	flagModel_.clear();
}

void EnemyPopManager::LoadPopdata() {

	std::ifstream ifs;

	ifs.open(filePath);

	//ファイルオープン失敗処理
	if (ifs.fail()) {
		std::string message = "敵出現データ記載のjsonファイルが見つかりません！";
		MessageBoxA(nullptr, message.c_str(), "ModelManager", 0);
		assert(message.c_str());
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//移したのでファイルを閉じる
	ifs.close();

	//サイズを変更
	popDatas_.resize(areaNum);

	//データ探索と作成
	int iterator = 0;
	for (auto& areaName : popAreaName) {
		//グループ検索
		nlohmann::json::iterator itGroup = root.find(areaName);

		//未登録チェック
		assert(itGroup != root.end());

		//構造体作成
		PopData popData{};

		popData.areaName = areaName;
		//各アイテムについて
		for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
			//アイテムの名前を取得
			const std::string& itemName = itItem.key();

			//各種データ取得
			if (itemNames[(int)INE::POSITION] == itemName) {
				popData.areaPosition = Vector3(itItem->at(0), itItem->at(1), itItem->at(2));
			}
			else if (itemNames[(int)INE::POPAREASIZE] == itemName) {
				popData.popAreaSize = itItem->get<float>();
			}
			else if (itemNames[(int)INE::RANGEPOPSTART] == itemName) {
				popData.PopStartingArea = itItem->get<float>();
			}
			else if (itemNames[(int)INE::POPINTERVAL] == itemName) {
				popData.PopInterval = itItem->get<float>();
			}
			else if (itemNames[(int)INE::MAXAREAPOP] == itemName) {
				popData.maxAreaPop = itItem->get<float>();
			}

		}

		popData.PopIntervalCount = 0;
		popData.maxAreaPopCount = 0;

		//データ挿入
		popDatas_[iterator] = popData;

		iterator++;
	}

	flagWorlds_.resize(popDatas_.size());

	int Index = 0;
	for (auto& popData : popDatas_) {
		//スポーン地点の座標指定
		EulerWorldTransform newW;
		newW.translate_ = popData.areaPosition;
		flagWorlds_.push_back(newW);

		//旗も追加
		std::unique_ptr<GameObject>newdata = std::make_unique<GameObject>();
		newdata->Initialize("flag");
		newdata->world_.translate_ = popData.areaPosition;
		newdata->world_.translate_.y += 6.0f;
		//値をもとに末尾に直接構築
		flagModel_.push_back(std::move(newdata));

		std::unique_ptr<GameObject>newdataaa = std::make_unique<GameObject>();
		newdataaa->Initialize("Flag");
		newdataaa->world_.translate_ = popData.areaPosition;
		//newdataaa->world_.translate_.y += 6.0f;
		newdataaa->model_->materialData_->enableEnvironmentMap = true;
		newdataaa->model_->materialData_->enviromentCoefficient = 0.1f;
		newdataaa->model_->materialData_->shininess = 0;
		//値をもとに末尾に直接構築
		poleModel_.push_back(std::move(newdataaa));


		//もう使わない
		//newdata.reset();

		Index++;
	}
}

void EnemyPopManager::LoadMapItem(const std::string& tag, Leveldata* datas)
{
	//データのサイズ分メモリ確保
	popDatas_.reserve(datas->items.size());
	flagWorlds_.reserve(popDatas_.size());

	//読み込まれたデータから出現データ作成
	int Index = 0;
	for (auto& data : datas->items) {
		//タグと同じ値の時入植
		if (data.itemname == tag) {
			PopData newData;
			//各値設定
			newData.areaPosition = data.transform.translate_;
			newData.areaName = data.itemname;
			newData.popAreaSize = data.floatValue_[itemNames[(int)INE::POPAREASIZE]];
			newData.PopStartingArea = data.floatValue_[itemNames[(int)INE::RANGEPOPSTART]];
			newData.PopInterval = data.floatValue_[itemNames[(int)INE::POPINTERVAL]];
			newData.maxAreaPop = data.floatValue_[itemNames[(int)INE::MAXAREAPOP]];


			//スポーン地点の座標指定
			EulerWorldTransform newW;
			newW.translate_ = newData.areaPosition;
			flagWorlds_.push_back(newW);

			//旗も追加
			std::unique_ptr<GameObject>newdata = std::make_unique<GameObject>();
			newdata->Initialize("flag");
			newdata->world_.translate_ = newData.areaPosition;
			newdata->world_.translate_.y += 6.0f;
			//値をもとに末尾に直接構築
			flagModel_.push_back(std::move(newdata));

			std::unique_ptr<GameObject>newdataaa = std::make_unique<GameObject>();
			newdataaa->Initialize("Flag");
			newdataaa->world_.translate_ = newData.areaPosition;
			//newdataaa->world_.translate_.y += 6.0f;
			newdataaa->model_->materialData_->enableEnvironmentMap = true;
			newdataaa->model_->materialData_->enviromentCoefficient = 0.1f;
			newdataaa->model_->materialData_->shininess = 0;
			//値をもとに末尾に直接構築
			poleModel_.push_back(std::move(newdataaa));

			//データ群に追加
			popDatas_.push_back(newData);

			//指定タグ分数増加
			Index++;
		}


	}
}

void EnemyPopManager::Initialzie() {
	for (auto& popdata : popDatas_) {
		popdata.PopIntervalCount = 0;
		popdata.maxAreaPopCount = 0;
	}

	InstancingModelManager* IMM = InstancingModelManager::GetInstance();

	IMM->SetTexture(flag_, TextureManager::white_);

	IMM->SetEnableEnviomentMap(flag_, true);
}

void EnemyPopManager::Update() {


	int Index = 0;
	for (auto& world : flagWorlds_) {
		if (popDatas_[Index].maxAreaPopCount < popDatas_[Index].maxAreaPop) {
			world.UpdateMatrix();

			flagModel_[Index]->Update();
			poleModel_[Index]->Update();
		}
		Index++;
	}

}

void EnemyPopManager::Draw() {
	InstancingModelManager* IMM = InstancingModelManager::GetInstance();
	int Index = 0;
	for (auto& world : flagWorlds_) {
		world.UpdateMatrix();
		if (popDatas_[Index].maxAreaPopCount < popDatas_[Index].maxAreaPop) {
			//IMM->SetData(flag_, world);
			poleModel_[Index]->Draw(TextureManager::white_);
			flagModel_[Index]->Draw(TextureManager::white_);
		}
		Index++;
	}
}

std::unique_ptr<ALEnemy> EnemyPopManager::PopEnemy() {

	//プレイヤー登録しているかチェック
	if (playerWorld_) {

		//出現エリアごとに処理
		for (auto& popdata : popDatas_) {

			//インターバルが０以下＆出現量が最大を超えていない
			if (popdata.PopIntervalCount-- <= 0 && popdata.maxAreaPopCount < popdata.maxAreaPop) {

				//出現エリアとの距離測定
				float length = Length(playerWorld_->GetMatWorldTranslate() - popdata.areaPosition);

				//エリア内で出現
				if (length <= popdata.PopStartingArea) {

					float area = popdata.popAreaSize;

					//乱数取得クラス呼び出し
					RandomNumber* rNum = RandomNumber::GetInstance();

					//出現座標作成
					Vector3 newPos{
						popdata.areaPosition.x + rNum->Get(-area,area),
						popdata.areaPosition.y + spawnHeight,
						popdata.areaPosition.z + rNum->Get(-area,area),
					};

					//生成インターバルを更新
					popdata.PopIntervalCount = popdata.PopInterval;

					//エリア出現最大数を加算
					popdata.maxAreaPopCount++;

					//新しく作成
					std::unique_ptr<ALEnemy>newEnemy = std::make_unique<ALEnemy>();
					newEnemy->Initialize(newPos, playerWorld_);

					//返却
					return std::move(newEnemy);
				}

			}
		}

	}

	//生成出来ず
	return nullptr;
}