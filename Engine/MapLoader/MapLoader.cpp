#include "MapLoader.h"
#include"Log/Log.h"
#include<fstream>
#include<json.hpp>
#include<iostream>
#include<cassert>
#include<numbers>
#include"Camera/Camera.h"
MapLoader* MapLoader::GetInstance()
{
	static MapLoader ins;
	return &ins;
}

void MapLoader::Initialize()
{
	levelDatas_.clear();
}

float Dig2Rad(float dig) {
	return dig * ((float)std::numbers::pi / 180);
}

void LoadMap(nlohmann::json& deserialized, std::vector<MapLoader::ObjectData>& objects, std::string tag) {
	for (nlohmann::json& object : deserialized[tag]) {
		assert(object.contains("type"));
		//種別を取得
		std::string type = object["type"].get<std::string>();
		//MESHがある場合
		if (type.compare("MESH") == 0) {
			//要素追加
			objects.emplace_back(MapLoader::ObjectData{});

			MapLoader::ObjectData& obData = objects.back();
			//名前があったら追加
			if (object.contains("file_name")) {
				//ファイル名
				obData.filename = object["file_name"];
			}
			//トランスフォーム値取得
			nlohmann::json& transform = object["transform"];
			//平行移動
			obData.transform.translate_.x = (float)transform["translation"][0];
			obData.transform.translate_.y = (float)transform["translation"][2];
			obData.transform.translate_.z = (float)transform["translation"][1];
			//回転
			obData.transform.rotate_.x = Dig2Rad( - (float)transform["rotation"][0]);
			obData.transform.rotate_.y = Dig2Rad( - (float)transform["rotation"][2]);
			obData.transform.rotate_.z = Dig2Rad( - (float)transform["rotation"][1]);

			

			//スケーリング
			obData.transform.scale_.x = (float)transform["scaling"][0];
			obData.transform.scale_.y = (float)transform["scaling"][2];
			obData.transform.scale_.z = (float)transform["scaling"][1];

			//コライダー
			
			if(object.contains("collider") ){
				nlohmann::json& collider = object["collider"];

				obData.collider.type = collider["type"];
				//中心点
				obData.collider.center.x = (float)collider["center"][0];
				obData.collider.center.y = (float)collider["center"][2];
				obData.collider.center.z = (float)collider["center"][1];
				//size
				obData.collider.size.x = (float)collider["size"][0];
				obData.collider.size.y = (float)collider["size"][2];
				obData.collider.size.z = (float)collider["size"][1];
			}

			if (object.contains("children")) {
				nlohmann::json& child = object["children"];
				LoadMap(object, obData.child, "children");
			}

			//オブジェクトタグ走査
			for (auto it = object.begin(); it != object.end();++it) {
				
				std::string prop = it.key();
				//Float型単品ならその型群に
				if (object[prop].is_number_float()) {
					//データ登録処理
					obData.floatValue_[prop] = it.value();
				}
				else if(object[prop].is_array()&&object[prop].size()==1) {
					Vector2 newd = { object[prop][0],object[prop][1] };
					obData.v2Value_[prop] = newd;
				}
				else if (object[prop].is_array() && object[prop].size() == 2) {
					Vector3 newd = { object[prop][0],object[prop][1],object[prop][2] };
					obData.v3Value_[prop] = newd;
				}
			}
		}
	}
}


void MapLoader::LoadLevelEditor(const std::string& filename, const std::string& kExtension)
{
#pragma region 読み込み処理
	const std::string fullpath = baseDirectory_ + filename + kExtension;

	//ファイルストリーム
	std::ifstream file;

	file.open(fullpath);
	//失敗チェック
	if (file.fail()) {
		assert(0);
	}

	//文字列から回答したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());
#pragma endregion

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);


	//レベルデータ格納用インスタンスを取得
	Leveldata* leveldata = new Leveldata();


	LoadMap(deserialized, leveldata->objects, "objects");



	levelDatas_.emplace_back(leveldata);
}


void MapLoader::CreateModel(int mapNum)
{
	stageNum_ = mapNum;
	LoadModel(levelDatas_[mapNum]->objects);
}

std::vector<Vector3> MapLoader::LoadObjectPosition(int mapNum)
{
	std::vector<Vector3>ans;

	for (auto& data : levelDatas_[mapNum]->objects) {
		ans.emplace_back(data.transform.translate_);
	}

	return ans;
}

void MapLoader::UpdateLevelData()
{
	for (auto& data : models_[stageNum_]) {
		data->Update();
	}

	for (auto& data : colliders_) {
		data->Update();
	}
}

void MapLoader::DrawLevelData()
{
	for (auto& data : models_[stageNum_]) {
		data->Draw();
	}
}

Vector3 MapLoader::IsCollisionMap(SphereCollider* collider)
{
	Vector3 ans{};

	Vector3 backVec;
	for (auto& cdata : colliders_) {
		cdata->IsCollision(collider, backVec);
		ans = backVec;
	}

	return ans;
}

std::optional<float> MapLoader::GetObjectFloatValue(const std::string filename, const std::string valueName)
{
	for (auto& object : levelDatas_[stageNum_]->objects) {
		if (object.filename == filename) {

			auto it = object.floatValue_.find(valueName);
			if (it != object.floatValue_.end()) {
				//キーが存在する場合
				return it->second;
			}
			else {
				//データが見つからない！
				assert(false);
				return std::nullopt;
			}
		}
	}
	//データが見つからない！
	assert(false);
	return std::nullopt;
}

std::optional<Vector2> MapLoader::GetObjectVec2Value(const std::string filename, const std::string valueName)
{
	for (auto& object : levelDatas_[stageNum_]->objects) {
		if (object.filename == filename) {

			auto it = object.v2Value_.find(valueName);
			if (it != object.v2Value_.end()) {
				//キーが存在する場合
				return it->second;
			}
			else {
				//データが見つからない！
				assert(false);
				return std::nullopt;
			}
		}
	}
	//データが見つからない！
	assert(false);
	return std::nullopt;
}

std::optional<Vector3> MapLoader::GetObjectVec3Value(const std::string filename, const std::string valueName)
{
	for (auto& object : levelDatas_[stageNum_]->objects) {
		if (object.filename == filename) {

			auto it = object.v3Value_.find(valueName);
			if (it != object.v3Value_.end()) {
				//キーが存在する場合
				return it->second;
			}
			else {
				//データが見つからない！
				assert(false);
				return std::nullopt;
			}
		}
	}
	//データが見つからない！
	assert(false);
	return std::nullopt;
}

void MapLoader::LoadModel(const std::vector<ObjectData>&d)
{
	//今は一番前の飲み読み込み
	for (auto& data : d) {
		std::unique_ptr<InstancingGameObject>obj = std::make_unique<InstancingGameObject>();
		obj->Initialize(data.filename);
		obj->world_ = data.transform;
		

		models_[stageNum_].push_back(std::move(obj));
		if (data.child.size() != 0) {
			LoadModel(data.child);
		}

		//こライダーデータ
		if (data.collider.type == "BOX") {
			std::unique_ptr<OBBCollider>newC = std::make_unique<OBBCollider>();
			newC->Initialize("sphere");
			newC->SetParent(&models_[stageNum_].back()->world_);
			newC->Update();

			colliders_.push_back(std::move(newC));
		}
	}
}
