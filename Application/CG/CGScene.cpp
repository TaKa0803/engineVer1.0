#include "CGScene.h"
#include"TextureManager/TextureManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include"MapLoader/MapLoader.h"

CGScnene::CGScnene()
{

	input_ = Input::GetInstance();
	camera_ = Camera::GetInstance();

	object = std::make_unique<GameObject>();
	terrain = new GameObject();

	//ball = TextureManager::LoadTex("resources/Texture/CG/rostock_laage_airport_4k.dds").texNum;

	skybox_ = new SkyBoxModel("resources/Texture/CG/rostock_laage_airport_4k.dds");

	MapLoader::GetInstance()->LoadLevelEditor("untitled",".json");
	MapLoader::GetInstance()->CreateModel(0);
}

CGScnene::~CGScnene() { 
	delete terrain;
	delete skybox_; 
}

void CGScnene::Initialize()
{
	object->Initialize("AnimeCube");
	//object->Initialize("sphere");
	//object->Initialize("walk");
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

	pointLight_.position += move;

	object->Update();
	terrain->Update();
	skybox_->Update();
	camera_->Update();

	MapLoader::GetInstance()->UpdateLevelData();

	Debug();
}

void CGScnene::PostEffectDraw()
{
	
	object->Draw(*camera_, pointLight_);
	terrain->Draw(*camera_, pointLight_);

	skybox_->Draw(camera_);

	MapLoader::GetInstance()->DrawLevelData();

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

	ImGui::Text("PointLight");
	ImGui::DragFloat("p light intencity", &pointLight_.intensity, 0.01f);
	ImGui::DragFloat3("p light pos", &pointLight_.position.x, 0.1f);
	ImGui::ColorEdit4("light color", &pointLight_.color.x);
	ImGui::DragFloat("p light radius", &pointLight_.radius, 0.1f);
	ImGui::DragFloat("p light decay", &pointLight_.decay, 0.1f);
}
