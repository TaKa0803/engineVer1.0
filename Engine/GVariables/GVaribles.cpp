#include "GVaribles.h"
#include"ImGuiManager/ImGuiManager.h"

GVariables* GVariables::GetInstance()
{
	static GVariables ins;
	return &ins;
}

void ItemImGui(const std::string name, std::variant<int32_t, float, Vector3>& value) {
	//intの場合
	if (std::holds_alternative<int32_t>(value)) {
		int32_t* ptr = std::get_if<int32_t>(&value);
		ImGui::DragInt(name.c_str(), ptr);
	}//floatの場合
	else if (std::holds_alternative<float>(value)) {
		float* ptr = std::get_if<float>(&value);
		ImGui::DragFloat(name.c_str(), ptr, 0.01f);
	}//Vector3の場合
	else if (std::holds_alternative<Vector3>(value)) {
		Vector3* ptr = std::get_if<Vector3>(&value);
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
				return TreeImGui(tree.first.c_str(), tree.second);
			}
		}
		ImGui::TreePop();
	}

	return;
}


void GVariables::Update()
{

#ifdef _DEBUG
	ImGui::Begin(baseName_.c_str());
	if (ImGui::BeginTabBar("LWP")) {
		//各グループで表示
		for (auto& data : datas_) {
			//タブを作成
			if (ImGui::BeginTabItem(data.first.c_str())) {

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
						return TreeImGui(tree.first.c_str(), tree.second);
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
