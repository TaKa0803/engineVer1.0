#pragma once
#include"Vector3.h"

#include<variant>
#include<string>
#include<map>


struct ItemData {
	//値
	std::variant<int32_t*, float*, Vector3*> value;	

};

struct TreeData {
	//値
	std::map<std::string, ItemData>value;

	//ツリーデータ
	std::map<std::string, TreeData>tree;
};

struct GroupData
{
	//値
	std::map<std::string, ItemData>data;
	//ツリーデータ
	std::map<std::string, TreeData>tree;

};
