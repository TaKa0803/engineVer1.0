#pragma once
#include<string>
#include"GVariableManager/GVariDatas.h"

//Gvariツリークラス
class GVariTree {

public:

	//ツリーデータ作成
	GVariTree(const std::string& treeName);
	~GVariTree() = default;

	void SetName(const std::string& name);

	//値追加
	void SetValue(const std::string& name, std::variant<int32_t*, float*, Vector3*> value);

	//監視値追加
	void SetMonitorValue(const std::string& name, std::variant<bool* ,int32_t*, float*, Vector3*, std::string*> value);

	//ツリーデータ格納
	void SetTreeData(GVariTree& tree);
public:

	//ツリー名
	std::string treeName_;

	//データ
	TreeData datas_;

private:



};

//Gvariグループクラス
class GVariGroup {

public:
	GVariGroup(const std::string&groupName);
	//削除時データを送る
	~GVariGroup();


	//値追加
	void SetValue(const std::string& name, std::variant<int32_t*, float*, Vector3*> value);

	//監視値追加
	void SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value);


	void SetTreeData(GVariTree& tree);
private:

	//グループ名
	std::string groupName_;

	//データ
	GroupData datas_;


};

