#include "ALBoss.h"

ALBoss::ALBoss()
{
	//一回しかしない初期化情報
	input_ = Input::GetInstance();
	input_->SetDeadLine(0.3f);

	GameObject::Initialize("Player");
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);

	
	shadow = std::make_unique<InstancingGameObject>();
	shadow->Initialize("DZone");
	shadow->SetParent(&world_);
	shadow->SetColor({ 0,0,0,1 });
	shadow->SetTranslate({ 0,0.01f,0 });
	shadow->SetScale(1.5f);
}


ALBoss::~ALBoss()
{
}

void ALBoss::Initilaize()
{
	world_.Initialize();


}

void ALBoss::Update()
{

	GameObject::Update();
}

void ALBoss::Draw()
{
	GameObject::Draw();
}
