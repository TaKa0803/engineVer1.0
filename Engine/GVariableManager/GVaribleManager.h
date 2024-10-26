#pragma once
#include"GVariDatas.h"

//すべてのGVariablesの管理  
class GlobalVariableManager {
	
public://シングルトン
	static GlobalVariableManager* GetInstance();

private:
	GlobalVariableManager() = default;
	~GlobalVariableManager() = default;
	GlobalVariableManager(const GlobalVariableManager& o) = delete;
	const GlobalVariableManager& operator=(const GlobalVariableManager& o) = delete;

public://パブリック関数

	//値の追加
	void SetGroup(const std::string& group,GroupData&data);

	//更新処理
	void Update();

	//セーブデータすべて読み込み
	void LoadAllSaveData();

	//全セーブデータのセット
	void SetLoadData();

	//グループデータ読み込み
	void SetLoadGroupData(const std::string& groupName);
private:

	//グループデータデータ保存
	void SaveGroupData(const std::string& groupName);

	//ファイルよみこみ(１ファイル
	void LoadGroupData(const std::string& groupName);
private:
	std::string baseName_ = "Game";



	std::map<std::string, GroupData> datas_;

	std::map<std::string, GroupData> saveDatas_;


	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
};