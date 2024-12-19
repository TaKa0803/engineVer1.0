#pragma once
#include"GlobalVariable/Tree/GlobalVariableTree.h"
#include"GlobalVariable/GlobalVariableDatas.h"

//グローバル値をまとめたグループ
class GlobalVariableGroup {

public: //**パブリック関数**//

	/// <summary>
	/// 生成（日本語だと出力名がバグるので注意)
	/// </summary>
	/// <param name="groupName"></param>
	GlobalVariableGroup(const std::string& groupName);
	//削除時データを送る
	~GlobalVariableGroup();

	/// <summary>
	/// デバッグ値追加
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="value">値</param>
	void SetValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*> value);

	/// <summary>
	/// 保存されないデバッグ値の追加
	/// </summary>
	/// <param name="name">値の名前</param>
	/// <param name="value">値</param>
	void SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector3*, std::string*> value);

	/// <summary>
	/// ImGuiのコンボでデバッグ
	/// </summary>
	/// <param name="name">コンボ名</param>
	/// <param name="item">変更する値</param>
	/// <param name="items">文字列群</param>
	void SetMonitorCombo(const std::string& name, std::string* item, const std::vector<std::string>& items);

	/// <summary>
	/// ツリーデータ格納
	/// </summary>
	/// <param name="tree">ツリーデータ</param>
	void SetTreeData(GlobalVariableTree& tree);

private: //**プライベート変数**//

	//グループ名
	std::string groupName_;

	//データ
	GroupData datas_;
};

/// <summary>///GlobalVariableGroupの別名 /// </summary>
using GVariGroup = GlobalVariableGroup;