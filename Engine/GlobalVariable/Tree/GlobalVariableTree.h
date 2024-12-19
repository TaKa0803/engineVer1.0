#pragma once
#include"GlobalVariable/GlobalVariableDatas.h"


//グローバル値ツリークラス
class GlobalVariableTree {

public: //**パブリック関数**//

	//ツリーデータ作成
	GlobalVariableTree(const std::string& treeName);
	~GlobalVariableTree() = default;

	//値追加
	void SetValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*> value);

	//監視値追加
	void SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value);

	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string& name, std::string* item, const std::vector<std::string>& items);

	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string& name, int32_t* item, const std::vector<std::string>& items);


	//ツリーデータ格納
	void SetTreeData(GlobalVariableTree& tree);

public: //**パブリック変数**//

	//ツリー名
	std::string name_;

	//データ
	TreeData datas_;
};

/// <summary>
/// GlobalVariableTreeの別名
/// </summary>
using GvariTree = GlobalVariableTree;
