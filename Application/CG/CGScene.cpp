#include "CGScene.h"
#include"TextureManager/TextureManager.h"

CGScnene::CGScnene()
{

	input_ = Input::GetInstance();
	camera_ = std::make_unique<Camera>();

	object = std::make_unique<GameObject>();
	terrain = new GameObject();

	ball = TextureManager::LoadTex("resources/Texture/SystemResources/monsterBall.png");
}

CGScnene::~CGScnene() { delete terrain; }

void CGScnene::Initialize()
{
	object->Initialize("walk");
	terrain->Initialize("terrain");

	camera_->Initialize();
	camera_->SetTarget(&object->GetWorld());
}

void CGScnene::Update()
{

	Vector3 move = input_->GetAllArrowKey();
	move.y = input_->GetWASD().z;

	pointLightPos_ += move;

	object->Update();
	terrain->Update();
	camera_->Update();

	Debug();
}

void CGScnene::PostEffectDraw()
{
}

void CGScnene::Draw()
{
	object->Draw(*camera_,pointLightPos_,ball);
	//terrain->Draw(*camera_, pointLightPos_);

	InstancingModelManager::GetInstance()->DrawAllModel(camera_->GetViewProjectionMatrix());
}

void CGScnene::Debug()
{
	object->Debug("object");
	//terrain->Debug("terrain");
	camera_->DrawDebugWindow("camera");
}
