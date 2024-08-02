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

	virtual void Debug(const char* name);
};

