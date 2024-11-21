#include "GVaribleManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include<json.hpp>
#include<fstream>
#include <iostream>

void ItemImGui(const std::string name, std::variant<bool*,int32_t*, float*, Vector3*, Vector4*> value) {


	//intの場合
	if (std::holds_alternative<bool*>(value)) {
		bool* ptr = *std::get_if<bool*>(&value);
		ImGui::Checkbox(name.c_str(), ptr);
	}else if (std::holds_alternative<int32_t*>(value)) {
		int32_t* ptr = *std::get_if<int32_t*>(&value);
		ImGui::DragInt(name.c_str(), ptr);
	}//floatの場合
	else if (std::holds_alternative<float*>(value)) {
		float* ptr = *std::get_if<float*>(&value);
		ImGui::DragFloat(name.c_str(), ptr, 0.01f);
	}//Vector3の場合
	else if (std::holds_alternative<Vector3*>(value)) {
		Vector3* ptr = *std::get_if<Vector3*>(&value);
		ImGui::DragFloat3(name.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
	}
	else if (std::holds_alternative<Vector4*>(value)) {
		Vector4* ptr = *std::get_if<Vector4*>(&value);
		ImGui::ColorEdit4(name.c_str(), &ptr->x);
	}
}

void MonitorItemImGui(const std::string name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value) {


	//intの場合
	if (std::holds_alternative<bool*>(value)) {
		bool* ptr = *std::get_if<bool*>(&value);
		ImGui::Checkbox(name.c_str(), ptr);
	}
	else if (std::holds_alternative<int32_t*>(value)) {
		int32_t* ptr = *std::get_if<int32_t*>(&value);
		std::string text = name + " : %d";
		ImGui::Text(text.c_str(), *ptr);
	}//floatの場合
	else if (std::holds_alternative<float*>(value)) {
		float* ptr = *std::get_if<float*>(&value);
		std::string text = name + " : %4.1f";
		ImGui::Text(text.c_str(), *ptr, 0.01f);
	}//Vector3の場合
	else if (std::holds_alternative<Vector3*>(value)) {
		Vector3* ptr = *std::get_if<Vector3*>(&value);
		std::string text = name + " :  %4.1f / %4.1f / %4.1f";
		ImGui::Text(text.c_str(), *reinterpret_cast<float*>(ptr), 0.01f);
	}
	else if (std::holds_alternative<std::string*>(value)) {
		// char* の場合
		std::string* ptr = *std::get_if<std::string*>(&value);
		std::string text = name + " : %s";
		ImGui::Text(text.c_str(), ptr->c_str());
	}
}

void TreeImGui(const std::string& name, TreeData& treeData) {

	if (ImGui::TreeNode(name.c_str())) {

		//モニター値の表示
		if (!treeData.monitorKeys.empty()) {
			if (ImGui::TreeNode("--モニター値---")) {
				//ツリー内に含まれるvalue登場
				for (auto& key : treeData.monitorKeys) {

					//タブ値の名前
					std::string name = key;
					//値
					MonitorItemData& item = treeData.monitorValue[key];


					//値の条件で処理変化
					MonitorItemImGui(name, item.value);
				}


				ImGui::TreePop();
			}
		}

		if (!treeData.valueKeys.empty()) {
			if (ImGui::TreeNode("--パラメータ---")) {

				//ツリー内に含まれるvalue登場
				for (auto& key : treeData.valueKeys) {

					//タブ値の名前
					std::string name = key;
					//値
					ItemData& item = treeData.value[key];


					//値の条件で処理変化
					ItemImGui(name, item.value);
				}

				ImGui::TreePop();
			}
		}


		//子ツリーの存在検知
		if (treeData.tree.size() != 0) {

			if (ImGui::TreeNode("--子ツリー--")) {

				//子ツリーの表示
				for (auto& key : treeData.treeKeys) {
					TreeImGui(key, treeData.tree[key]);
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	return;
}


GlobalVariableManager* GlobalVariableManager::GetInstance()
{
	static GlobalVariableManager ins;
	return &ins;
}



void GlobalVariableManager::SetGroup(const std::string& group, GroupData& data)
{
	datas_[group] = data;
}

void GlobalVariableManager::Update()
{

#ifdef _DEBUG
	saveDatas_;
	ImGui::Begin(baseName_.c_str());
	if (ImGui::BeginTabBar("LWP")) {
		//各グループで表示
		for (auto& data : datas_) {
			//タブを作成
			if (ImGui::BeginTabItem(data.first.c_str())) {
				//保存処理
				if (ImGui::Button("パラメータの保存")) {
					SaveGroupItemData(data.first);
					std::string message = std::format("{}.json saved.", data.first);
					MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
				}

				//読み込み処理
				if (ImGui::Button("保存されてるパラメータの読み込み")) {
					LoadAllSaveData();
					SetLoadGroupData(data.first);
					std::string message = std::format("{}.json loaded.", data.first);
					MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
				}


				if (!data.second.monitorKeys.empty()) {
					if (ImGui::TreeNode("--モニター値--")) {

						//ツリー内に含まれるvalue登場
						for (auto& key : data.second.monitorKeys) {

							//タブ値の名前
							std::string name = key;
							//値
							MonitorItemData& item = data.second.monitorData[key];


							//値の条件で処理変化
							MonitorItemImGui(name, item.value);
						}
						ImGui::TreePop();
					}
				}

				if (!data.second.valueKeys.empty()) {
					if (ImGui::TreeNode("--パラメータ--")) {
						//タブの値を表示
						for (auto& key : data.second.valueKeys) {

							//タブ値の名前
							std::string name = key;
							//値
							ItemData& item = data.second.value[key];

							//値の条件で処理変化
							ItemImGui(name, item.value);

						}
						ImGui::TreePop();
					}
				}


				//子ツリーの存在検知
				if (data.second.tree.size() != 0) {
					if (ImGui::TreeNode("--子ツリー--")) {
						//子ツリーの表示
						for (auto& key : data.second.treeKeys) {
							TreeImGui(key, data.second.tree[key]);
						}
						ImGui::TreePop();
					}
				}

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
#endif // _DEBUG



}


void SaveItemData(
	nlohmann::json& root,
	std::map<std::string, ItemData> data) {
	//各項目について
	for (std::map<std::string, ItemData>::iterator itItem = data.begin();
		itItem != data.end(); ++itItem) {

		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		ItemData& item = itItem->second;
		
		if (std::holds_alternative<bool*>(item.value)) {//int型の値を保持していれば
			//int32_t型の値を登録
			root[itemName] = *std::get<bool*>(item.value);

		}else if (std::holds_alternative<int32_t*>(item.value)) {//int型の値を保持していれば
			//int32_t型の値を登録
			root[itemName] = *std::get<int32_t*>(item.value);

		}
		else if (std::holds_alternative<float*>(item.value)) {//float型の値を保持していれば
			// int32_t型の値を登録
			root[itemName] = *std::get<float*>(item.value);
		}
		else if (std::holds_alternative<Vector3*>(item.value)) {//Vector3型の値を保持していれば
			Vector3 value = *std::get<Vector3*>(item.value);
			root[itemName] = nlohmann::json::array({ value.x, value.y, value.z });
		}
		else if (std::holds_alternative<Vector4*>(item.value)) {//Vector3型の値を保持していれば
			Vector4 value = *std::get<Vector4*>(item.value);
			root[itemName] = nlohmann::json::array({ value.x, value.y, value.z,value.w });
		}

	}
}

/// <summary>
/// ツリーの再帰的処理
/// </summary>
/// <param name="keyName">元グループ、ツリー名</param>
/// <param name="jsonNode">参照元のノード</param>
/// <param name="treeData">参照データ</param>
void ProcessTree(nlohmann::json& jsonNode, const std::map<std::string, TreeData>& treeData) {

	for (const auto& tree : treeData) {
		// 新しいノードを作成
		nlohmann::json newNode = nlohmann::json::object();


		//ツリー名追加
		std::string treename = tree.first;


		//valueの処理
		SaveItemData(newNode, tree.second.value);

		// 再帰的にtreeを処理
		if (!tree.second.tree.empty()) {
			//ツリーのツリー処理
			ProcessTree(newNode, tree.second.tree);
		}

		// ノードを親のjsonに追加
		jsonNode[treename] = newNode;
	}
}

void GlobalVariableManager::SaveGroupItemData(const std::string& groupName)
{
	//グループを検索
	std::map<std::string, GroupData>::iterator itGroup = datas_.find(groupName);
	//未登録チェック
	assert(itGroup != datas_.end());

	nlohmann::json root;
	root = nlohmann::json::object();

	//jsonオブジェスト登録
	root[groupName] = nlohmann::json::object();

	//valueの処理
	SaveItemData(root[groupName], itGroup->second.value);
	//ツリーの処理
	ProcessTree(root[groupName], itGroup->second.tree);

	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists("Resources/GlobalVariables")) {
		std::filesystem::create_directory("Resources/GlobalVariables");
	}



	//書き込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + groupName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filepath);

	//ファイルオープン失敗?
	if (ofs.fail()) {
		std::string message = "Failed open file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}
	//ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}


void SetLoadTreeData(TreeData& groupData, SavedTreeData& saveData) {

	//現在データからループで参照(value
	for (auto itItem = groupData.value.begin(); itItem != groupData.value.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		//	セーブデータに同名のデータを検索
		if (saveData.value.end() != saveData.value.find(itemname)) {

			//値取得
			std::variant<bool,int32_t, float, Vector3,Vector4>& saveV = saveData.value[itemname].value;
			std::variant<bool*,int32_t*, float*, Vector3*, Vector4*>& dataV = groupData.value[itemname].value;

			//合致する値を保存
			if (std::holds_alternative<bool>(saveV) && std::holds_alternative<bool*>(dataV)) {
				bool savePtr = *std::get_if<bool>(&saveV);
				bool* dataPtr = *std::get_if<bool*>(&dataV);
				*dataPtr = savePtr;
			}else if (std::holds_alternative<int32_t>(saveV) && std::holds_alternative<int32_t*>(dataV)) {
				int32_t savePtr = *std::get_if<int32_t>(&saveV);
				int32_t* dataPtr = *std::get_if<int32_t*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<float>(saveV) && std::holds_alternative<float*>(dataV)) {
				float savePtr = *std::get_if<float>(&saveV);
				float* dataPtr = *std::get_if<float*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector3>(saveV) && std::holds_alternative<Vector3*>(dataV)) {
				Vector3 savePtr = *std::get_if<Vector3>(&saveV);
				Vector3* dataPtr = *std::get_if<Vector3*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector4>(saveV) && std::holds_alternative<Vector4*>(dataV)) {
				Vector4 savePtr = *std::get_if<Vector4>(&saveV);
				Vector4* dataPtr = *std::get_if<Vector4*>(&dataV);
				*dataPtr = savePtr;
			}
		}
	}

	//ツリーの処理
	for (auto itItem = groupData.tree.begin(); itItem != groupData.tree.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		if (saveData.tree.end() != saveData.tree.find(itemname)) {
			SetLoadTreeData(groupData.tree[itemname], saveData.tree[itemname]);
		}
	}
}

void GlobalVariableManager::SetLoadGroupData(const std::string& groupName)
{
	GroupData& groupdata = datas_[groupName];

	SavedGroupData& saveData = saveDatas_[groupName];

	//同グループ名所持が一致しない場合
	if (datas_.end() == datas_.find(groupName) || saveDatas_.end() == saveDatas_.find(groupName)) {
		return;
	}

	//現在データからループで参照(value
	for (auto itItem = groupdata.value.begin(); itItem != groupdata.value.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		//	セーブデータに同名のデータを検索
		if (saveData.value.end() != saveData.value.find(itemname)) {

			//値取得
			std::variant<bool,int32_t, float, Vector3,Vector4>& saveV = saveData.value[itemname].value;
			std::variant<bool*,int32_t*, float*, Vector3*, Vector4*>& dataV = groupdata.value[itemname].value;

			//合致する値を保存
			if (std::holds_alternative<bool>(saveV) && std::holds_alternative<bool*>(dataV)) {
				bool savePtr = *std::get_if<bool>(&saveV);
				bool* dataPtr = *std::get_if<bool*>(&dataV);
				*dataPtr = savePtr;
			}else if (std::holds_alternative<int32_t>(saveV) && std::holds_alternative<int32_t*>(dataV)) {
				int32_t savePtr = *std::get_if<int32_t>(&saveV);
				int32_t* dataPtr = *std::get_if<int32_t*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<float>(saveV) && std::holds_alternative<float*>(dataV)) {
				float savePtr = *std::get_if<float>(&saveV);
				float* dataPtr = *std::get_if<float*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector3>(saveV) && std::holds_alternative<Vector3*>(dataV)) {
				Vector3 savePtr = *std::get_if<Vector3>(&saveV);
				Vector3* dataPtr = *std::get_if<Vector3*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector4>(saveV) && std::holds_alternative<Vector4*>(dataV)) {
				Vector4 savePtr = *std::get_if<Vector4>(&saveV);
				Vector4* dataPtr = *std::get_if<Vector4*>(&dataV);
				*dataPtr = savePtr;
			}
		}

	}
	//同名ツリー処理
		//ツリーの処理
	for (auto itItem = groupdata.tree.begin(); itItem != groupdata.tree.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		if (saveData.tree.end() != saveData.tree.find(itemname)) {
			SetLoadTreeData(itItem->second, saveData.tree[itemname]);
		}
	}
}


void LoadTreeData(SavedTreeData& treeData, const nlohmann::json& jsonNode) {
	// 各アイテムについて
	for (auto itItem = jsonNode.begin(); itItem != jsonNode.end(); ++itItem) {
		const std::string& itemName = itItem.key();

		SavedItemData& data = treeData.value[itemName];

		// int 型を保持している場合
		if (itItem->is_boolean()) {
			//値取得
			bool value = itItem->get<bool>();
			data.value = value;
		}
		else if (itItem->is_number_integer()) {
			//値取得
			int32_t value = itItem->get<int32_t>();
			data.value = value;
		}
		// float 型を保持している場合
		else if (itItem->is_number_float()) {
			float value = itItem->get<float>();
			data.value = value;

		}// Vector3 (3 要素の配列) を保持している場合
		else if (itItem->is_array() && itItem->size() == 3) {
			Vector3 value = { itItem->at(0).get<float>(), itItem->at(1).get<float>(), itItem->at(2).get<float>() };
			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 4) {
			Vector4 value = { itItem->at(0).get<float>(), itItem->at(1).get<float>(), itItem->at(2).get<float>(),itItem->at(3).get<float>() };
			data.value = value;
		}
		else if (itItem->is_object()) {
			// 子ツリーを再帰的に処理
			SavedTreeData& newTree = treeData.tree[itemName];
			LoadTreeData(newTree, *itItem);
		}

	}
}

void GlobalVariableManager::LoadGroupData(const std::string& groupName)
{

	//よみこむJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + groupName + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に聞く
	ifs.open(filepath);

	// ファイルオープン失敗?
	if (ifs.fail()) {
		std::string message = "Failed open file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータの構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	saveDatas_[groupName].value.clear();
	saveDatas_[groupName].tree.clear();


	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		SavedItemData& data = saveDatas_[groupName].value[itemName];

		//int型を保持していた場合
		if (itItem->is_boolean()) {
			bool value = itItem->get<bool>();
			data.value = value;
		}else if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();
			data.value = value;
		}
		else if (itItem->is_number_float()) {// float型を保持していた場合
			float value = itItem->get<float>();

			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 3) { // 要素数３の配列であれば
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };

			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 4) { // 要素数３の配列であれば
			Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2),itItem->at(3)};

			data.value = value;
		}       // 子ツリーの読み込み処理
		else if (itItem->is_object()) {
			LoadTreeData(saveDatas_[groupName].tree[itemName], *itItem);
		}
	}

}

void GlobalVariableManager::LoadAllSaveData()
{
	// ディレクトリがなければ返す
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists("Resources/GlobalVariables/")) {
		return;
	}

	std::filesystem::directory_iterator dir_it("Resources/GlobalVariables");
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filepath = entry.path();

		//ファイル拡張子を取得
		std::string extention = filepath.extension().string();
		//.jsonファイル以外はスキップ
		if (extention.compare(".json") != 0) {
			continue;
		}
		LoadGroupData(filepath.stem().string());

	}
}

void GlobalVariableManager::SetLoadAllData()
{

	for (auto& data : datas_) {
		std::string name = data.first;

		SetLoadGroupData(name);
	}


}
