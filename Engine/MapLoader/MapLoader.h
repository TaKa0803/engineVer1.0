#pragma once
#include<string>
#include<list>
#include<vector>

#include"WorldTransform/WorldTransform.h"
#include"SingleGameObjects/GameObject.h"
#include"ColliderOBB/OBBCollider.h"
#include"InstancingGameObject/InstancingGameObject.h"
#include"SphereCollider/SphereCollider.h"

//コライダーデータ
struct ColliderData
{
	std::string type;//コライダータイプ
	Vector3 center;	//中央座標
	Vector3 size;	//サイズ
};

//オブジェクトのデータ
struct ObjectData {
	std::string filename;			//名前
	EulerWorldTransform transform;	//座標
	ColliderData collider;			//コライダー
	std::vector<ObjectData>child;	//子供
	std::map<std::string, float>floatValue_;	//カスタムパラメータ（float
	std::map<std::string, Vector2>v2Value_;	//カスタムパラメータ（Vec2
	std::map<std::string, Vector3>v3Value_;	//カスタムパラメータ（Vec3

};

//アイテムデータ
struct Itemdata {
	std::string itemname;			//名前
	EulerWorldTransform transform;	//座標
	std::map<std::string, float>floatValue_;//カスタムパラメータ（float
	std::map<std::string, Vector2>v2Value_;//カスタムパラメータ（Vec2
	std::map<std::string, Vector3>v3Value_;//カスタムパラメータ（Vec3
};

//レベルデータ統括
struct Leveldata {
	std::vector<ObjectData>objects;
	std::vector<Itemdata>items;
};

//マップ読み込みクラス
class MapLoader {
public://**シングルトンパターン**//
	static MapLoader* GetInstance();
private:
	MapLoader() = default;
	~MapLoader() = default;
	MapLoader(const MapLoader& o) = delete;
	const MapLoader& operator=(const MapLoader& o) = delete;

public://**パブリック関数**//

	/// <summary>
	/// ステージデータなど含め初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// マップロード
	/// </summary>
	/// <param name="filename">フォルダパス</param>
	/// <param name="kExtension">名前</param>
	void LoadLevelEditor(const std::string& filename, const std::string& kExtension);
	
	/// <summary>
	/// データからモデル生成
	/// </summary>
	/// <param name="mapNum">マップ番号</param>
	void CreateModel(int mapNum);

	/// <summary>
	/// 座標取得
	/// </summary>
	/// <param name="mapNUm">マップ番号</param>
	/// <returns>座標群</returns>
	std::vector<Vector3>LoadObjectPosition(int mapNUm);

	
	/// <summary>
	///　読み込まれたマップ番号のマップを更新 
	/// </summary>
	void UpdateLevelData();

	/// <summary>
	/// マップ番号のマップを描画
	/// </summary>
	void DrawLevelData();

	/// <summary>
	/// マップ全体との当たり判定処理
	/// </summary>
	/// <param name="collider">判定するコライダー</param>
	/// <param name="backV">押し出しベクトル</param>
	/// <returns></returns>
	bool IsCollisionMap(SphereCollider* collider,Vector3&backV );

	/// <summary>
	/// レベルデータ取得
	/// </summary>
	/// <returns></returns>
	Leveldata* GetLevelData() { return levelDatas_[stageNum_]; };

	/// <summary>
	/// すべてのコライダー取得
	/// </summary>
	/// <returns></returns>
	std::vector<std::unique_ptr<OBBCollider>>& GetColliderData() { return colliders_; }

private://**プライベート関数**//

	/// <summary>
	/// モデルの読み込み
	/// </summary>
	/// <param name="d"></param>
	void LoadModel(const std::vector<ObjectData>&d);

	//objectのカスタムデータ取得
	std::optional<float> GetObjectFloatValue(const std::string filename, const std::string valueName);

	//objectのカスタムデータ取得
	std::optional<Vector2> GetObjectVec2Value(const std::string filename, const std::string valueName);

	//objectのカスタムデータ取得
	std::optional<Vector3> GetObjectVec3Value(const std::string filename, const std::string valueName);


private://**プライベート変数**//

	//現在のステージ番号
	int stageNum_;

	//ベースフォルダまでのパス
	std::string baseDirectory_="resources/levelEditor/";

	//レベルデータ
	std::vector<Leveldata*>levelDatas_;

	//<マップ番号、マップでのモデルデータ群>
	std::map<int,std::vector<std::unique_ptr<InstancingGameObject>>>models_;

	//OBBコライダー
	std::vector<std::unique_ptr<OBBCollider>>colliders_;

	//描画フラグ
	bool isDraw_ = true;

	//コライダー描画フラグ
	bool isDrawCollider_ = true;
};