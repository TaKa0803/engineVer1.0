#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"


#include<variant>
#include<string>
#include<vector>
#include<map>

struct ItemData {
	//値
	std::variant<bool*,int32_t*, float*,Vector2*, Vector3*,Vector4*> value;	
};

struct MonitorItemData {
	//値
	std::variant<bool*, int32_t*, float*, Vector3*,std::string*> value;

	//コンボ時利用
	std::vector<std::string> items;
};



struct TreeData {
	//値
	std::map<std::string, ItemData>value;

	//監視する値
	std::map<std::string, MonitorItemData>monitorValue;

	//ツリーデータ
	std::map<std::string, TreeData>tree;

	//順に表示する用のキー
	std::vector<std::string>valueKeys;
	std::vector<std::string>monitorKeys;
	std::vector<std::string>treeKeys;

};

struct GroupData
{
	//値
	std::map<std::string, ItemData>value;

	//監視する値
	std::map<std::string, MonitorItemData>monitorValue;

	//ツリーデータ
	std::map<std::string, TreeData>tree;

	//順に表示する用のキー
	std::vector<std::string>valueKeys;
	std::vector<std::string>monitorKeys;
	std::vector<std::string>treeKeys;

};


struct SavedItemData {
	//値
	std::variant<bool,int32_t, float,Vector2, Vector3,Vector4> value;
};

struct SavedTreeData
{
	//値
	std::map<std::string, SavedItemData>value;

	//ツリーデータ
	std::map<std::string, SavedTreeData>tree;
};

struct SavedGroupData {
	//値
	std::map<std::string, SavedItemData>value;

	//ツリーデータ
	std::map<std::string, SavedTreeData>tree;
};