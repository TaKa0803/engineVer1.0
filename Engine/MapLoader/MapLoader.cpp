#include "MapLoader.h"
#include"Log/Log.h"
#include<fstream>
#include<json.hpp>
#include<iostream>
#include<cassert>
#include<numbers>
#include"Camera/Camera.h"
#include"ImGuiManager/ImGuiManager.h"
MapLoader* MapLoader::GetInstance()
{
	//インスタンス生成
	static MapLoader ins;
	return &ins;
}

void MapLoader::Initialize()
{
	//レベルデータのクリア
	levelDatas_.clear();
}

/// <summary>
/// DigreeをRadianに変換
/// </summary>
/// <param name="dig">digree</param>
/// <returns></returns>
float Dig2Rad(float dig) {
	return dig * ((float)std::numbers::pi / 180);
}

/// <summary>
/// マップ読み込み
/// </summary>
/// <param name="deserialized">jsonデータ</param>
/// <param name="objects">データを入れるところ</param>
/// <param name="tag">タグ</param>
void LoadMap(nlohmann::json& deserialized, std::vector<ObjectData>& objects, std::string tag) {
	//タグのオブジェクト取得
	for (nlohmann::json& object : deserialized[tag]) {
		//typeがなければ取得
		assert(object.contains("type"));
		//種別を取得
		std::string type = object["type"].get<std::string>();

		//MESHがある場合
		if (type.compare("MESH") == 0) {
			
			//要素追加
			objects.emplace_back(ObjectData{});
			ObjectData& obData = objects.back();

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
				//データをいれるところ参照
				nlohmann::json& collider = object["collider"];
				//type取得
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

			//オブジェクトタグ走査
			for (auto it = object.begin(); it != object.end();++it) {

				//キー取得
				std::string prop = it.key();

				//どれかでも一致する場合流す
				if (prop == "collider" || prop == "file_name" || prop == "transform") {
					continue;
				}

				//トランスフォーム値取得
				nlohmann::json& transform2 = object["transform"];
				//平行移動
				obData.transform.translate_.x = (float)transform2["translation"][0];
				obData.transform.translate_.y = (float)transform2["translation"][2];
				obData.transform.translate_.z = (float)transform2["translation"][1];
				//回転
				obData.transform.rotate_.x = Dig2Rad(-(float)transform2["rotation"][0]);
				obData.transform.rotate_.y = Dig2Rad(-(float)transform2["rotation"][2]);
				obData.transform.rotate_.z = Dig2Rad(-(float)transform2["rotation"][1]);

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

			//子供がいる場合
			if (object.contains("children")) {
				//nlohmann::json& child = object["children"];
				//再帰処理
				LoadMap(object, obData.child, "children");
			}
		}		
	}
}

/// <summary>
/// カスタムデータの読み込み
/// </summary>
/// <param name="deserialized">元データ</param>
/// <param name="objects">データを入れるところ</param>
/// <param name="tag">タグ</param>
void LoadItem(nlohmann::json& deserialized, std::vector<Itemdata>& objects, std::string tag) {
	//タグのところをすべて参照
	for (nlohmann::json& object : deserialized[tag]) {
		//typeがない場合エラー
		assert(object.contains("type"));
		//種別を取得
		std::string type = object["type"].get<std::string>();

		//指定名前がある場合
		if (type.compare("SpawnPoint")==0) {
			//要素追加
			objects.emplace_back(Itemdata{});
			Itemdata& itData = objects.back();

			//名前があったら追加
			if (object.contains("file_name")) {
				//ファイル名
				itData.itemname = object["file_name"];
			}

			//オブジェクトタグ走査
			for (auto it = object.begin(); it != object.end(); ++it) {

				//キー取得
				std::string prop = it.key();

				//いずれか一致する場合スキップ
				if (prop == "collider" || prop == "file_name" || prop == "transform") {
					continue;
				}

				//トランスフォーム値取得
				nlohmann::json& transform = object["transform"];
				//平行移動
				itData.transform.translate_.x = (float)transform["translation"][0];
				itData.transform.translate_.y = (float)transform["translation"][2];
				itData.transform.translate_.z = (float)transform["translation"][1];
				//回転
				itData.transform.rotate_.x = Dig2Rad(-(float)transform["rotation"][0]);
				itData.transform.rotate_.y = Dig2Rad(-(float)transform["rotation"][2]);
				itData.transform.rotate_.z = Dig2Rad(-(float)transform["rotation"][1]);

				//Float型単品ならその型群に
				if (object[prop].is_number_float()) {
					//データ登録処理
					itData.floatValue_[prop] = it.value();
				}
				else if (object[prop].is_array() && object[prop].size() == 1) {
					Vector2 newd = { object[prop][0],object[prop][1] };
					itData.v2Value_[prop] = newd;
				}
				else if (object[prop].is_array() && object[prop].size() == 2) {
					Vector3 newd = { object[prop][0],object[prop][1],object[prop][2] };
					itData.v3Value_[prop] = newd;
				}
			}
		}
	}
}

void MapLoader::LoadLevelEditor(const std::string& filename, const std::string& kExtension)
{
#pragma region 読み込み処理
	//フルパス作成
	const std::string fullpath = baseDirectory_ + filename + kExtension;

	//ファイルストリーム
	std::ifstream file;

	//ファイルp－分
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

	//オブジェクトの座標、回転、サイズ取得
	LoadMap(deserialized, leveldata->objects, "objects");
	//カスタムパラメータ追加
	LoadItem(deserialized, leveldata->items, "objects");

	//データの保存
	levelDatas_.emplace_back(leveldata);
}


void MapLoader::CreateModel(int mapNum)
{
	//ステージ番号登録
	stageNum_ = mapNum;
	//モデルをすべて読み込む
	LoadModel(levelDatas_[mapNum]->objects);
}

std::vector<Vector3> MapLoader::LoadObjectPosition(int mapNum)
{
	std::vector<Vector3>ans;

	//すべてのオブジェクトの座標のみ保存
	for (auto& data : levelDatas_[mapNum]->objects) {
		ans.emplace_back(data.transform.translate_);
	}

	return ans;
}

void MapLoader::UpdateLevelData()
{
	//モデルの更新
	for (auto& data : models_[stageNum_]) {
		data->Update();
	}

	//コライダーの更新
	for (auto& data : colliders_) {
		data->Update();
	}

#ifdef _DEBUG
	ImGui::Begin("maploader");
	ImGui::Checkbox("drawM", &isDraw_);
	ImGui::Checkbox("drawC", &isDrawC_);

	ImGui::End();
#endif // _DEBUG

}

void MapLoader::DrawLevelData()
{
	//描画フラグが有効の場合
	if (isDraw_) {
		for (auto& data : models_[stageNum_]) {
			data->Draw();
		}
	}

	//コライダー描画フラグが有効の場合
	if (isDrawCollider_) {
		for (auto& data : colliders_) {
			if (data->isActive_) {
				data->Draw();
			}
		}
	}
}

bool MapLoader::IsCollisionMap(SphereCollider* collider,Vector3& backV)
{
	//処理をおこなったらフラグ
	bool ans = false;

	//すべてのコライダーとの当たり判定
	Vector3 backVec;
	for (auto& cdata : colliders_) {
		if (cdata->isActive_&&cdata->IsCollision(collider, backVec, 5)) {
			backV += backVec;
			ans= true;
		}
	}

	return ans;
}

std::optional<float> MapLoader::GetObjectFloatValue(const std::string filename, const std::string valueName)
{
	//ステージ番号のオブジェクト取得
	for (auto& object : levelDatas_[stageNum_]->objects) {
		//名前が一致した場合
		if (object.filename == filename) {
			//値名取得
			auto it = object.floatValue_.find(valueName);
			//見つかった場合
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
	//ステージ番号のオブジェクト取得
	for (auto& object : levelDatas_[stageNum_]->objects) {
		//名前が一致した場合
		if (object.filename == filename) {
			//値名取得
			auto it = object.v2Value_.find(valueName);
			//見つかった場合
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
	//ステージ番号のオブジェクト取得
	for (auto& object : levelDatas_[stageNum_]->objects) {
		//名前が一致した場合
		if (object.filename == filename) {
			//値名取得
			auto it = object.v3Value_.find(valueName);
			//見つかった場合
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
		//インスタンスオブジェクト生成
		std::unique_ptr<InstancingGameObject>obj = std::make_unique<InstancingGameObject>();
		//初期化
		obj->Initialize(data.filename);
		//座標設定
		obj->world_ = data.transform;
		//値設定
		models_[stageNum_].push_back(std::move(obj));

		//子がいるかチェック
		if (data.child.size() != 0) {
			LoadModel(data.child);
		}

		//こライダーデータ
		if (data.collider.type == "BOX") {
			//コライダー生成
			std::unique_ptr<OBBCollider>newC = std::make_unique<OBBCollider>();
			newC->Initialize(data.filename+" c");
			newC->SetParent(&models_[stageNum_].back()->world_);
			newC->Update();
			colliders_.push_back(std::move(newC));
		}
	}
}
