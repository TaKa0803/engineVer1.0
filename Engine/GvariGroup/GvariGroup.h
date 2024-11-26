#pragma once
#include<string>
#include"GVariableManager/GVariDatas.h"

//Gvariツリークラス
class GVariTree {

public:

	//ツリーデータ作成
	GVariTree(const std::string& treeName);
	~GVariTree() = default;

	/// <summary>
	/// ツリー名設定
	/// </summary>
	/// <param name="name"></param>
	void SetName(const std::string& name);

	//値追加
	void SetValue(const std::string& name, std::variant<bool* ,int32_t*, float*, Vector3*, Vector4*> value);

	//監視値追加
	void SetMonitorValue(const std::string& name, std::variant<bool* ,int32_t*, float*, Vector3*, std::string*> value);



	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string&name,  std::string*item,const std::vector<std::string>& items);

	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string& name, int32_t* item, const std::vector<std::string>& items);


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
	/// <summary>
	/// 生成（日本語だと出力名がバグるので注意)
	/// </summary>
	/// <param name="groupName"></param>
	GVariGroup(const std::string&groupName);
	//削除時データを送る
	~GVariGroup();


	//デバッグ値追加
	void SetValue(const std::string& name, std::variant<bool*,int32_t*, float*, Vector3*, Vector4*> value);

	/// <summary>
	/// 保存されないデバッグ値の追加
	/// </summary>
	/// <param name="name">値の名前</param>
	/// <param name="value">値</param>
	void SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value);
	
	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string& name, std::string* item, const std::vector<std::string>& items);

	/// <summary>
	/// ツリーデータ格納
	/// </summary>
	/// <param name="tree"></param>
	void SetTreeData(GVariTree& tree);
private:

	//グループ名
	std::string groupName_;

	//データ
	GroupData datas_;


};


