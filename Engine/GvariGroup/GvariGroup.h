#pragma once
#include<string>
#include"GVariableManager/GlobalVariableDatas.h"

//Gvariツリークラス
class GlobalVariableTree {

public: //**パブリック関数**//

	//ツリーデータ作成
	GlobalVariableTree(const std::string& treeName);
	~GlobalVariableTree() = default;

	//値追加
	void SetValue(const std::string& name, std::variant<bool* ,int32_t*, float*,Vector2*, Vector3*, Vector4*> value);

	//監視値追加
	void SetMonitorValue(const std::string& name, std::variant<bool* ,int32_t*, float*, Vector3*, std::string*> value);

	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string&name,  std::string*item,const std::vector<std::string>& items);

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

//Gvariグループクラス
class GlobalVariableGroup {

public: //**パブリック関数**//

	/// <summary>
	/// 生成（日本語だと出力名がバグるので注意)
	/// </summary>
	/// <param name="groupName"></param>
	GlobalVariableGroup(const std::string&groupName);
	//削除時データを送る
	~GlobalVariableGroup();

	//デバッグ値追加
	void SetValue(const std::string& name, std::variant<bool*,int32_t*, float*,Vector2*, Vector3*, Vector4*> value);

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
	void SetTreeData(GlobalVariableTree& tree);

private: //**プライベート変数**//

	//グループ名
	std::string groupName_;

	//データ
	GroupData datas_;
};


