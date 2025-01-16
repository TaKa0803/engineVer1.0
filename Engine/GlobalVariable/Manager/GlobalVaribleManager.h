#pragma once
#include"GlobalVariable/GlobalVariableDatas.h"


//すべてのGlobalVariableの管理  
class GlobalVariableManager {
	
public: //**シングルトン**//
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス返却</returns>
	static GlobalVariableManager* GetInstance();
private:
	GlobalVariableManager() = default;
	~GlobalVariableManager() = default;
	GlobalVariableManager(const GlobalVariableManager& o) = delete;
	const GlobalVariableManager& operator=(const GlobalVariableManager& o) = delete;

public://パブリック関数

	/// <summary>
	/// グループの追加
	/// </summary>
	/// <param name="group">グループデータ名</param>
	/// <param name="data">グループデータ</param>
	void SetGroup(const std::string& group,GroupData&data);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// Jsonに保存されているデータを読み込み
	/// </summary>
	void LoadAllSaveData();

	/// <summary>
	/// 読み込まれているセーブデータを今のパラメータに読み込む
	/// </summary>
	void SetLoadAllData();

	/// <summary>
	/// グループのセーブデータのセット
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void SetLoadGroupData(const std::string& groupName);

	/// <summary>
	/// セットしたデータの削除
	/// </summary>
	void ClearSetData();

private: //**プライベート関数**//

	/// <summary>
	/// グループデータの保存
	/// </summary>
	/// <param name="groupName"></param>
	void SaveGroupItemData(const std::string& groupName);

	/// <summary>
	/// グループデータの読み込み
	/// </summary>
	/// <param name="groupName"></param>
	void LoadGroupData(const std::string& groupName);

private: //**プライベート変数**//

	//基本ウィンドウの名前
	std::string baseName_ = "Game";
	
	//グループのデータ群
	std::map<std::string, GroupData> datas_;

	//保存データ群
	std::map<std::string, SavedGroupData> saveDatas_;

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	//ノード表記にまとめ始める量
	size_t nodeSize_ = 4;
};

/// <summary>
/// GlobalVariablesManagerの別名
/// </summary>
using GvariM = GlobalVariableManager;
