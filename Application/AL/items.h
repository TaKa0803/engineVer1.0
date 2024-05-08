#pragma once
#include"WorldTransform/WorldTransform.h"


//モデルの数
static const int modelNum_ = 5;


struct ModelAnimeParts {
	int RoopFrame;
	EulerWorldTransform stPartsWorlds[modelNum_];
	EulerWorldTransform edPartsWorlds[modelNum_];
};
