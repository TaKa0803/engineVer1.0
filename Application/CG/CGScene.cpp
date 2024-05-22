#include "CGScene.h"
#include"TextureManager/TextureManager.h"

CGScnene::CGScnene()
{

	input_ = Input::GetInstance();
	camera_ = std::make_unique<Camera>();

	object = std::make_unique<GameObject>();
	terrain = new GameObject();

	//ball = TextureManager::LoadTex("resources/Texture/CG/rostock_laage_airport_4k.dds").texNum;

	skybox_ = new SkyBoxModel("resources/Texture/CG/rostock_laage_airport_4k.dds");
}

CGScnene::~CGScnene() { 
	delete terrain;
	delete skybox_; 
}

void CGScnene::Initialize()
{
	object->Initialize("AnimeCube");
	//object->Initialize("sphere");
	terrain->Initialize("terrain");

	camera_->Initialize();
	camera_->SetTarget(&object->GetWorld());

	skybox_->Initialize();
	skybox_->world_.scale_ = { 500,500,500 };
}

void CGScnene::Update()
{

	Vector3 move = input_->GetAllArrowKey();
	move.y = input_->GetWASD().z;

	pointLightPos_ += move;

	object->Update();
	terrain->Update();
	skybox_->Update();
	camera_->Update();

	Debug();
}

void CGScnene::PostEffectDraw()
{
	
	object->Draw(*camera_, pointLightPos_);
	terrain->Draw(*camera_, pointLightPos_);

	skybox_->Draw(camera_.get());

	InstancingModelManager::GetInstance()->DrawAllModel(camera_->GetViewProjectionMatrix());
	
}

void CGScnene::Draw()
{
	
}

void CGScnene::Debug()
{
	object->Debug("object");
	terrain->Debug("terrain");
	camera_->DrawDebugWindow("camera");
}
