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

void GameObject::Draw( PointLight pointlight, int tex) {

	model_->Draw(world_.matWorld_,pointlight,tex);
}

void GameObject::Debug(const char* name)
{
	if (ImGui::BeginMenu(name)) {

		model_->DebugParameter(name);

		world_.DrawDebug(name);

		ImGui::EndMenu();
	}
}

