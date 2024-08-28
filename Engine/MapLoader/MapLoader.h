#pragma once
#include<string>
#include<list>
#include<vector>
#include"WorldTransform/WorldTransform.h"
#include"SingleGameObjects/GameObject.h"
#include"ColliderOBB/OBBCollider.h"
#include"InstancingGameObject/InstancingGameObject.h"
#include"SphereCollider/SphereCollider.h"


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
	std::map<std::string, float>floatValue_;
	std::map<std::string, Vector2>v2Value_;
	std::map<std::string, Vector3>v3Value_;

};

struct Itemdata {
	std::string itemname;
	EulerWorldTransform transform;
	std::map<std::string, float>floatValue_;
	std::map<std::string, Vector2>v2Value_;
	std::map<std::string, Vector3>v3Value_;
};

struct Leveldata {
	std::vector<ObjectData>objects;
	std::vector<Itemdata>items;
};

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

	//座標取得
	std::vector<Vector3>LoadObjectPosition(int mapNUm);

	//読み込まれたマップ番号のマップを更新
	void UpdateLevelData();

	//マップ番号のマップを描画
	void DrawLevelData();

	Vector3 IsCollisionMap(SphereCollider* collider);


	Leveldata* GetLevelData() { return levelDatas_[stageNum_]; };


private:
	void LoadModel(const std::vector<ObjectData>&d);

	//objectのカスタムデータ取得
	std::optional<float> GetObjectFloatValue(const std::string filename, const std::string valueName);

	//objectのカスタムデータ取得
	std::optional<Vector2> GetObjectVec2Value(const std::string filename, const std::string valueName);

	//objectのカスタムデータ取得
	std::optional<Vector3> GetObjectVec3Value(const std::string filename, const std::string valueName);


	//現在のステージ番号
	int stageNum_;

	//ベースフォルダまでのパス
	std::string baseDirectory_="resources/levelEditor/";

	std::vector<Leveldata*>levelDatas_;

	//<マップ番号、マップでのモデルデータ群>
	std::map<int,std::vector<std::unique_ptr<InstancingGameObject>>>models_;

	std::vector<std::unique_ptr<OBBCollider>>colliders_;

	bool isDraw_ = true;
	bool isDrawC_ = true;
};