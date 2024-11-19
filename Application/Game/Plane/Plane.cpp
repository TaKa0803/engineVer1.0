#include "Plane.h"
#include"GvariGroup/GvariGroup.h"
#include<imgui.h>

Plane::Plane() {
	GameObject::Initialize("plane");

	GVariGroup gvg = GVariGroup("Plane");
	gvg.SetTreeData(model_->SetDebugParam());
}

Plane::~Plane()
{
	
}



void Plane::Initialize() {
	
	world_.scale_ = { 500,500,500 };

	world_.UpdateMatrix();
}



void Plane::Draw() {
	world_.UpdateMatrix();
	GameObject::Draw();
}
