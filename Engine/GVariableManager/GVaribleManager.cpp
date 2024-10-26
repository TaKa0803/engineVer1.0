#include "GVaribleManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include<json.hpp>
#include<fstream>
#include <iostream>

void ItemImGui(const std::string name, std::variant<int32_t*, float*, Vector3*> value) {


	//intの場合
	if (std::holds_alternative<int32_t*>(value)) {
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
}

void TreeImGui(const std::string& name, TreeData& treeData) {

	if (ImGui::TreeNode(name.c_str())) {

		//ツリー内に含まれるvalue登場
		for (auto& tabValue : treeData.value) {

			//タブ値の名前
			std::string name = tabValue.first;
			//値
			ItemData& item = tabValue.second;


			//値の条件で処理変化
			ItemImGui(name, item.value);
		}


		//子ツリーの存在検知
		if (treeData.tree.size() != 0) {
			//子ツリーの表示
			for (auto& tree : treeData.tree) {
				TreeImGui(tree.first.c_str(), tree.second);
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
				if (ImGui::Button("Save")) {
					SaveGroupData(data.first);
					std::string message = std::format("{}.json saved.", data.first);
					MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
				}

				//読み込み処理
				if (ImGui::Button("LoadSaveData")) {
					LoadAllSaveData();
					SetLoadGroupData(data.first);
					std::string message = std::format("{}.json loaded.", data.first);
					MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
				}


				//タブの値を表示
				for (auto& tabValue : data.second.data) {

					//タブ値の名前
					std::string name = tabValue.first;
					//値
					ItemData& item = tabValue.second;

					//値の条件で処理変化
					ItemImGui(name, item.value);

				}

				//子ツリーの存在検知
				if (data.second.tree.size() != 0) {
					//子ツリーの表示
					for (auto& tree : data.second.tree) {
						TreeImGui(tree.first.c_str(), tree.second);
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

		if (std::holds_alternative<int32_t*>(item.value)) {//int型の値を保持していれば
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

void GlobalVariableManager::SaveGroupData(const std::string& groupName)
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
	SaveItemData(root[groupName], itGroup->second.data);
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

void GlobalVariableManager::SetLoadGroupData(const std::string& groupName)
{
	GroupData& groupdata = datas_[groupName];

	GroupData& saveData = saveDatas_[groupName];

	//現在データからループで参照(value
	for (auto itItem = groupdata.data.begin(); itItem != groupdata.data.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;
		//キーの存在チェック
		if (saveData.data.end() != saveData.data.find(itemname)) {

			std::variant<int32_t*, float*, Vector3*>& saveV = saveData.data[itemname].value;
			std::variant<int32_t*, float*, Vector3*>& dataV = groupdata.data[itemname].value;


			// sValueのポインタが指す値をvalueのポインタが指す先に代入
			//std::visit([](auto&& src, auto&& dest) {
			//	if constexpr (std::is_same_v<decltype(src), int32_t*> && std::is_same_v<decltype(dest), int32_t*>) {
			//		 *dest = src;
			//	}
			//	else if constexpr (std::is_same_v<decltype(src), float*> && std::is_same_v<decltype(dest), float*>) {
			//		 *dest = src;
			//	}
			//	else if constexpr (std::is_same_v<decltype(src), Vector3*> && std::is_same_v<decltype(dest), Vector3*>) {
			//		 *dest = src;
			//	}

			//}, saveV, groupdata.data[itemname].value);

			////intの場合
			//if (std::holds_alternative<int32_t*>(value)) {
			//	int32_t* ptr = *std::get_if<int32_t*>(&value);
			//	ImGui::DragInt(name.c_str(), ptr);
			//}//floatの場合
			//else if (std::holds_alternative<float*>(value)) {
			//	float* ptr = *std::get_if<float*>(&value);
			//	ImGui::DragFloat(name.c_str(), ptr, 0.01f);
			//}//Vector3の場合
			//else if (std::holds_alternative<Vector3*>(value)) {
			//	Vector3* ptr = *std::get_if<Vector3*>(&value);
			//	ImGui::DragFloat3(name.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
			//}

			if (std::holds_alternative<int32_t*>(saveV)&& std::holds_alternative<int32_t*>(dataV)) {
				int32_t* savePtr = *std::get_if<int32_t*>(&saveV);
				int32_t* dataPtr = *std::get_if<int32_t*>(&dataV);
				*dataPtr = *savePtr;
			}else if (std::holds_alternative<float*>(saveV) && std::holds_alternative<float*>(dataV)) {
				float* savePtr = *std::get_if<float*>(&saveV);
				float* dataPtr = *std::get_if<float*>(&dataV);
				*dataPtr = *savePtr;
			}
			else if (std::holds_alternative<Vector3*>(saveV) && std::holds_alternative<Vector3*>(dataV)) {
				Vector3* savePtr = *std::get_if<Vector3*>(&saveV);
				Vector3* dataPtr = *std::get_if<Vector3*>(&dataV);
				*dataPtr = *savePtr;
			}

		}

	}
}


void LoadTreeData(TreeData& treeData, const nlohmann::json& jsonNode) {
	// 各アイテムについて
	for (auto itItem = jsonNode.begin(); itItem != jsonNode.end(); ++itItem) {
		const std::string& itemName = itItem.key();

		// int 型を保持している場合
		if (itItem->is_number_integer()) {
			//値取得
			int32_t value = itItem->get<int32_t>();

			// ポインタで取得	
			int32_t* ptr = std::get<int32_t*>(treeData.value[itemName].value);
			if (ptr == nullptr) {
				ptr = new int32_t;  // メモリを動的に確保
			}
			*ptr = value;  // 値を代入
		}
		// float 型を保持している場合
		else if (itItem->is_number_float()) {
			float value = itItem->get<float>();
			float* ptr = std::get<float*>(treeData.value[itemName].value);
			if (ptr == nullptr) {
				ptr = new float;  // メモリを動的に確保
			}
			*ptr = value;  // 値を代入
		}// Vector3 (3 要素の配列) を保持している場合
		else if (itItem->is_array() && itItem->size() == 3) {
			Vector3 value = { itItem->at(0).get<float>(), itItem->at(1).get<float>(), itItem->at(2).get<float>() };

			Vector3* ptr = std::get<Vector3*>(treeData.value[itemName].value);
			if (ptr == nullptr) {
				ptr = new Vector3;  // メモリを動的に確保
			}
			*ptr = value;  // 値を代入

		}
		// 子ツリーの処理
		else if (itItem->is_object()) {
			// 子ツリーを再帰的に処理
			TreeData newTree;
			LoadTreeData(newTree, *itItem);
			treeData.tree[itemName] = newTree;
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

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();


		//int型を保持していた場合
		if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();

			// ポインタの初期化チェック
			if (std::holds_alternative<int32_t*>(saveDatas_[groupName].data[itemName].value)) {

				int32_t* ptr = std::get<int32_t*>(saveDatas_[groupName].data[itemName].value);
				if (ptr == nullptr) {
					ptr = new int32_t;  // メモリを動的に確保
				}
				*ptr = value;  // 値を代入
				saveDatas_[groupName].data[itemName].value = ptr;  // variantにポインタを再設定

			}


		}
		else if (itItem->is_number_float()) {// float型を保持していた場合
			double value = itItem->get<double>();
			float fValue = static_cast<float>(value);

			// ポインタの初期化チェック
			if (std::holds_alternative<float*>(saveDatas_[groupName].data[itemName].value)) {
				float* ptr = std::get<float*>(saveDatas_[groupName].data[itemName].value);
				if (ptr == nullptr) {
					ptr = new float;  // メモリを動的に確保
				}
				*ptr = fValue;  // 値を代入
				saveDatas_[groupName].data[itemName].value = ptr;  // variantにポインタを再設定
			}

		}
		else if (itItem->is_array() && itItem->size() == 3) { // 要素数３の配列であれば
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };

			// ポインタの初期化チェック
			if (std::holds_alternative<Vector3*>(saveDatas_[groupName].data[itemName].value)) {
				Vector3* ptr = std::get<Vector3*>(saveDatas_[groupName].data[itemName].value);
				if (ptr == nullptr) {
					ptr = new Vector3;  // メモリを動的に確保
				}
				*ptr = value;  // 値を代入
				saveDatas_[groupName].data[itemName].value = ptr;  // variantにポインタを再設定
			}


		}        // 子ツリーの読み込み処理
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

void GlobalVariableManager::SetLoadData()
{




}
