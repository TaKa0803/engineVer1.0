#include"WorldTransform.h"
#include"Math/Matrix.h"
#include<imgui.h>

void EulerWorldTransform::Initialize() {
	translate_ = { 0.0f,0.0f,0.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	scale_ = { 1.0f,1.0f,1.0f };
	parent_ = nullptr;

	UpdateMatrix();
}

Matrix4x4 EulerWorldTransform::UpdateMatrix() {
	matWorld_ = MakeAffineMatrix(scale_, rotate_, translate_);

	if (parent_) {
		matWorld_ = matWorld_* parent_->matWorld_;
	}

	return matWorld_;
}

void EulerWorldTransform::DrawDebug(const char*name) {
#ifdef _DEBUG

	if (ImGui::BeginMenu(name)) {
		ImGui::DragFloat3("座標", &translate_.x, 0.01f);
		ImGui::DragFloat3("回転", &rotate_.x, 0.01f);
		ImGui::DragFloat3("拡縮", &scale_.x, 0.01f);
		ImGui::EndMenu();
	}

#endif // _DEBUG
}

GVariTree& EulerWorldTransform::GetDebugTree(const std::string& name)
{
	tree_.SetName(name);

	tree_.SetValue("座標", &translate_);
	tree_.SetValue("回転", &rotate_);
	tree_.SetValue("スケール", &scale_);

	return tree_;
}







void QuaterinionWorldTransform::Initialize()
{
}

Matrix4x4 QuaterinionWorldTransform::UpdateMatrix()
{
	matWorld_ = MakeAffineMatrix(scale_, rotate_, translate_);

	if (parent_) {
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	return matWorld_;
}

void QuaterinionWorldTransform::DrawDebug(const char* name)
{
#ifdef _DEBUG
	if (ImGui::BeginMenu(name)) {
		ImGui::DragFloat3("座標", &translate_.x, 0.01f);
		ImGui::DragFloat3("回転", &rotate_.x, 0.01f);
		ImGui::DragFloat3("拡縮", &scale_.x, 0.01f);
		ImGui::EndMenu();
	}
#endif // _DEBUG
}
