#include "LightManager.h"

LightManager* LightManager::GetInstance()
{
	static LightManager instance;
	return&instance;
}

void LightManager::Initialize()
{
}

void LightManager::PreUpdate()
{
}
