#include "GameObject.h"
#include<imgui.h>


GameObject::~GameObject() {
	delete model_;
}

void GameObject::Initialize(const std::string& filepath) {
	if (model_ == nullptr) {
		model_ = Model::CreateFromOBJ(filepath);
	}
}

void GameObject::Update() {
	world_.UpdateMatrix();
	model_->UpdateAnimation();
}

void GameObject::Draw(  int tex) {

	model_->Draw(world_.matWorld_,tex);
}

void GameObject::Debug(const char* name)
{
	if (ImGui::BeginMenu(name)) {

		world_.DrawDebug(name);

		ImGui::EndMenu();
	}
}

