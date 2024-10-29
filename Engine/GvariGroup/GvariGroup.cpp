#include "GvariGroup.h"
#include"GVariableManager/GVaribleManager.h"

GVariGroup::GVariGroup(const std::string& groupName)
{
	groupName_ = groupName;
}

GVariGroup::~GVariGroup()
{
	//データをマネージャにセット
	GlobalVariableManager::GetInstance()->SetGroup(groupName_,datas_);

	//クリアする
	//datas_.data.clear();
	//datas_.tree.clear();
}

void GVariGroup::SetValue(const std::string& name, std::variant<int32_t*, float*, Vector3*> value)
{
	datas_.data[name].value = value;
	datas_.valueKeys.push_back(name);
}

void GVariGroup::SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value)
{
	datas_.monitorData[name].value = value;
	datas_.monitorKeys.push_back(name);
}




void GVariGroup::SetTreeData(GVariTree& tree)
{
	datas_.tree[tree.treeName_] = tree.datas_;
	datas_.treeKeys.push_back(tree.treeName_);
}


GVariTree::GVariTree(const std::string& treeName)
{
	treeName_ = treeName;
}

void GVariTree::SetName(const std::string& name)
{
	treeName_ = name;
}

void GVariTree::SetValue(const std::string& name, std::variant<int32_t*, float*, Vector3*> value)
{
	datas_.value[name].value = value;
	datas_.valueKeys.push_back(name);
}

void GVariTree::SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value)
{
	datas_.monitorValue[name].value = value;
	datas_.monitorKeys.push_back(name);
}



void GVariTree::SetTreeData(GVariTree& tree)
{
	datas_.tree[tree.treeName_] = tree.datas_;
	datas_.treeKeys.push_back(tree.treeName_);
}
