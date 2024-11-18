#include "SpriteManager.h"

SpriteManager* SpriteManager::GetInstance()
{
	static SpriteManager instance;
	return&instance;
}

void SpriteManager::Initialize()
{
	grarphics_ = std::make_unique<SpritePSO>();
	grarphics_->Initialize();
}

void SpriteManager::Finalize()
{
	//delete grarphics_;
	//grarphics_ = nullptr;
}


void SpriteManager::PreDraw(BlendMode blendMode) {
	SpriteManager::GetInstance()->grarphics_->PreDraw( blendMode);
}