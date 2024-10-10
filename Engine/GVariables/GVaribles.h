#pragma once
#include"Vector3.h"

#include<variant>
#include<string>
#include<map>

struct ItemData {
	//値
	std::variant<int32_t, float, Vector3>& value;
};

struct TreeData {
	//値
	std::map<std::string, ItemData>value;

	//ツリーデータ
	std::map<std::string, TreeData>tree;
};

class GVariables {
	
public://シングルトン
	static GVariables* GetInstance();

private:
	GVariables() = default;
	~GVariables() = default;
	GVariables(const GVariables& o) = delete;
	const GVariables& operator=(const GVariables& o) = delete;

public://パブリック関数

	//値の追加
	void SetValue(const std::string& group);

	//更新処理
	void Update();


private:

	std::string baseName_ = "Game";

	struct GroupData
	{
		//値
		std::map<std::string, ItemData>data;
		//ツリーデータ
		std::map<std::string, TreeData>tree;

	};

	std::map<std::string, GroupData> datas_;

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
};