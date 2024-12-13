#include "GvariGroup.h"
#include"GVariableManager/GlobalVaribleManager.h"
#include"Vector4.h"

GlobalVariableGroup::GlobalVariableGroup(const std::string& groupName)
{
	//グループ名設定
	groupName_ = groupName;
}
 
GlobalVariableGroup::~GlobalVariableGroup()
{
	//データをマネージャにセット
	GlobalVariableManager::GetInstance()->SetGroup(groupName_,datas_);
}

void GlobalVariableGroup::SetValue(const std::string& name, std::variant<bool*,int32_t*, float*,Vector2*, Vector3*, Vector4*> value)
{
	//値をセット
	datas_.value[name].value = value;
	datas_.valueKeys.push_back(name);
}

void GlobalVariableGroup::SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value)
{
	//監視値をセット
	datas_.monitorValue[name].value = value;
	datas_.monitorKeys.push_back(name);
}

void GlobalVariableGroup::SetMonitorCombo(const std::string& name, std::string* item, const std::vector<std::string>& items)
{
	//監視文字列をセット
	datas_.monitorValue[name].value = item;
	datas_.monitorKeys.push_back(name);
	datas_.monitorValue[name].items = items;
}

void GlobalVariableGroup::SetTreeData(GlobalVariableTree& tree)
{
	//ツリーデータをセット
	datas_.tree[tree.name_] = tree.datas_;
	datas_.treeKeys.push_back(tree.name_);
}


GlobalVariableTree::GlobalVariableTree(const std::string& treeName)
{
	//名前をセット
	name_ = treeName;
}

void GlobalVariableTree::SetValue(const std::string& name, std::variant<bool*,int32_t*, float*,Vector2*, Vector3*,Vector4*> value)
{
	//値をセット
	datas_.value[name].value = value;
	datas_.valueKeys.push_back(name);
}

void GlobalVariableTree::SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value)
{
	//監視値をセット
	datas_.monitorValue[name].value = value;
	datas_.monitorKeys.push_back(name);
}

void GlobalVariableTree::SetMonitorCombo(const std::string& name, std::string*item, const std::vector<std::string>& items)
{
	//監視文字列をセット
	datas_.monitorValue[name].value = item;
	datas_.monitorKeys.push_back(name);
	datas_.monitorValue[name].items = items;
}

void GlobalVariableTree::SetMonitorCombo(const std::string& name, int32_t* item, const std::vector<std::string>& items)
{
	//監視文字列をセット
	datas_.monitorValue[name].value = item;
	datas_.monitorKeys.push_back(name);
	datas_.monitorValue[name].items = items;
}

void GlobalVariableTree::SetTreeData(GlobalVariableTree& tree)
{
	//ツリーデータのセット
	datas_.tree[tree.name_] = tree.datas_;
	datas_.treeKeys.push_back(tree.name_);
}
