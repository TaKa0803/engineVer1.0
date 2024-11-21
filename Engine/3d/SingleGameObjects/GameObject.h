#pragma once

#include"SingleGameObjects/Model.h"
#include"WorldTransform/WorldTransform.h"
#include<vector>

class GameObject {

protected:

	

public:

	Model* model_ = nullptr;

	EulerWorldTransform world_;

	const EulerWorldTransform& GetWorld() { return world_; }

	virtual ~GameObject();

	virtual void Initialize(const std::string& filepath);

	virtual void Update();

	virtual void Draw( int tex = -1);

};

//if (ImGui::Combo("blendmode", &currentItem, items, IM_ARRAYSIZE(items))) {
//	blend = static_cast<BlendMode>(currentItem);
//}
//
//BlendMode blend = blendMode_;
//const char* items[] = { "None","Normal","Add","Subtract","Multiply","Screen" };
//int currentItem = static_cast<int>(blend);