#pragma once
#include<string>
#include<list>
#include<vector>
#include"WorldTransform/WorldTransform.h"
#include"SingleGameObjects/GameObject.h"
#include"ColliderOBB/OBBCollider.h"

class MapLoader {
public:
	static MapLoader* GetInstance();
private:
	MapLoader() = default;
	~MapLoader() = default;
	MapLoader(const MapLoader& o) = delete;
	const MapLoader& operator=(const MapLoader& o) = delete;

public:
	//ステージデータなど含め初期化
	void Initialize();
	//マップロード
	void LoadLevelEditor(const std::string& filename, const std::string& kExtension);
	//データからモデルを生成
	void CreateModel(int mapNum);

	//読み込まれたマップ番号のマップを更新
	void UpdateLevelData();

	//マップ番号のマップを描画
	void DrawLevelData();

	struct ColliderData
	{
		std::string type;
		Vector3 center;
		Vector3 size;
	};

	struct ObjectData {
		std::string filename;
		EulerWorldTransform transform;
		ColliderData collider;
		std::vector<ObjectData>child;
	};

	struct Leveldata {
		std::vector<ObjectData>objects;
	};

private:
	void LoadModel(std::vector<ObjectData>d);

	int stageNum_;

	//ベースフォルダまでのパス
	std::string baseDirectory_="resources/levelEditor/";

	std::vector<Leveldata*>levelDatas_;

	//<マップ番号、マップでのモデルデータ群>
	std::map<int,std::vector<std::unique_ptr<GameObject>>>models_;

	std::vector<std::unique_ptr<OBBCollider>>colliders_;
};