#include "GameObject.h"
#include<imgui.h>


GameObject::~GameObject() {
	//モデルの削除
	delete model_;
}

void GameObject::Initialize(const std::string& filepath) {
	//モデルがnullの場合
	if (model_ == nullptr) {
		//モデルを作成
		model_ = Model::CreateFromOBJ(filepath);
	}
}

void GameObject::Update() {
	//行列更新
	world_.UpdateMatrix();
}

void GameObject::Draw(int tex) {
	//描画
	model_->Draw(world_.matWorld_, tex);
}



