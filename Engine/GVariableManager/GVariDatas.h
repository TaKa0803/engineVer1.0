#pragma once
#include"Vector3.h"

#include<variant>
#include<string>
#include<vector>
#include<map>


struct ItemData {
	//値
	std::variant<int32_t*, float*, Vector3*> value;	
};

struct MonitorItemData {
	//値
	std::variant<bool*, int32_t*, float*, Vector3*,std::string*> value;
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
	std::map<std::string, ItemData>data;

	//監視する値
	std::map<std::string, MonitorItemData>monitorData;

	//ツリーデータ
	std::map<std::string, TreeData>tree;

	//順に表示する用のキー
	std::vector<std::string>valueKeys;
	std::vector<std::string>monitorKeys;
	std::vector<std::string>treeKeys;

};
