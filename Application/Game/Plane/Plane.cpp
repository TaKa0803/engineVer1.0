#include "Plane.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"
#include<imgui.h>

Plane::Plane() {
	//オブジェクト初期化
	GameObject::Initialize("plane");

	//デバッグ用パラ追加
	GlobalVariableGroup gvg = GlobalVariableGroup("Plane");
	gvg.SetTreeData(model_->SetDebugParam());
}

Plane::~Plane()
{
	
}

void Plane::Initialize() {
	//サイズ設定
	world_.scale_ = { 500,500,500 };
	world_.UpdateMatrix();
}

void Plane::Draw() {
	//行列更新
	world_.UpdateMatrix();
	//描画
	GameObject::Draw();
}
