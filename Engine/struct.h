#pragma once
#include<stdint.h>
#include<vector>
#include<string>
#include<map>
#include<optional>

#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Math/Vector4.h"
#include"Math/Matrix.h"

#include"WorldTransform/WorldTransform.h"

#pragma region 構造体

enum class BlendMode {
	//!<ブレンドなし
	kNone,
	//!<通常αブレンド
	kNormal,
	//!<加算
	kAdd,
	//!<減算
	kSubtract,
	//!<乗算
	kMultily,
	//!<スクリーン
	kScreen,
	//利用してはいけない
	kCountOfBlendMode
};

//塗りつぶし設定
enum class FillMode {
	//埋める
	kSolid,
	//ワイヤー
	kWireFrame,
	//使用不可
	kCountOfFillMode
};

struct WorldTransformation {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
	Vector4 color;
};

struct VertexData {
	Vector4 position = { 0,0,0,0 };
	Vector2 texcoord = { 0,0 };
	Vector3 normal = { 0,0,0 };
};

struct MaterialData {
	std::string textureFilePath;
};

struct Node {
	QuaterinionWorldTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node>children;
};

struct Joint {
	QuaterinionWorldTransform transform;	//Transform情報
	Matrix4x4 localMatrix;					//localMatrix
	Matrix4x4 skeletonSpaceMatrix;			//skeletonSpaceでの変換行列
	std::string name;						//名前
	std::vector<int32_t>children;			//子のjointIndexのリスト。いなければ空
	int32_t index;							//自身のIndex
	std::optional<int32_t>parent;			//親ジョイントのIndex。いなければnull
};

struct Skeleton {
	int32_t root;//rootJointのIndex
	std::map<std::string, int32_t>jointMap;
	std::vector<Joint>joints;
};

template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};
using KayframeVector3 = Keyframe<Vector3>;
using KayframeQuaternion = Keyframe<Quaternion>;

template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>>keyframes;
};

struct NodeAnimation
{
	AnimationCurve<Vector3>translate;
	AnimationCurve<Quaternion>rotate;
	AnimationCurve<Vector3>scale;
};

struct Animation
{
	//animation全体の尺
	float duration;
	//ノードanimation集合、Node名で引ける
	std::map<std::string, NodeAnimation>nodeAnimations;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};

struct ModelAllData
{
	ModelData model;
	Animation animation;
};

struct Material {
	//色
	Vector4 color;
	//UV座標
	Matrix4x4 uvTransform;

	int32_t enableLighting;
	//float padding[3];

	int32_t enableTexture;
	//float padding2[3];

	int32_t enableHalfLambert;
	//float padding3[3];

	float discardNum;
	//float padding4[3];

	int32_t enablePhongReflection = 1;

	//光沢度
	float shininess = 1.0f;

	int32_t enablePointLight=true;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
	//float padding2[3];
};

struct Camera4GPU {
	Vector3 worldPosition;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};


//ポイントライト
struct PointLight
{
	Vector4 color;//ライトの色
	Vector3 position;//座標
	float intensity;//光度
};

#pragma endregion
