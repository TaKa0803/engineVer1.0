#pragma once
#include "SingleGameObjects/GameObject.h"
#include"Camera/Camera.h"
class Plane :public GameObject {

public:

	Plane();
	~Plane();

	//void CaluclateInit();

	void Initialize();

	void Draw();


};

