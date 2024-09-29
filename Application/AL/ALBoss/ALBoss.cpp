#include "ALBoss.h"
#include"ImGuiManager/ImGuiManager.h"

ALBoss::ALBoss(ALPlayer* player)
{
	player_ = player;

	//一回しかしない初期化情報
	input_ = Input::GetInstance();
	input_->SetDeadLine(0.3f);

	GameObject::Initialize("box");
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);

	model_->IsEnableTexture(false);
	model_->SetColor({ 1,1,1,1 });


	collider_ = std::make_unique<SphereCollider>();
	collider_->Initialize("boss", world_);
	collider_->SetRadius(5.0f);
	collider_->SetTranslate({ 0,1.4f,0 });

	shadow_ = std::make_unique<CirccleShadow>(world_);

	idle_ = std::make_unique<BossIdle>(this);
	move_ = std::make_unique<BossMove>(this);
	atk_ = std::make_unique<BossATK>(this);


	atkCollider_ = std::make_unique<SphereCollider>();
	atkCollider_->Initialize("boss", world_);
	atkCollider_->SetRadius(5.0f);
	atkCollider_->SetTranslate({ 0.0f,1.4f,0.0f });
	atkCollider_->isActive_ = false;
}


ALBoss::~ALBoss()
{
}

void ALBoss::Initilaize()
{
	world_.Initialize();
	world_.translate_ = { 0,0,10 };
	world_.scale_ = { 5,5,5 };
	atk_->SceneInit();
}

void ALBoss::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Boss");
	ImGui::Text("behavior : %s", behaviorName_[(int)behavior_].c_str());
	ImGui::DragInt("体力:", &HP_);
	ImGui::End();

	model_->DebugParameter("Boss");
	collider_->Debug("Boss");
	atkCollider_->Debug("BossATK");
#endif // _DEBUG


	//状態の初期化処理
	if (behaviorReq_) {
		behavior_ = behaviorReq_.value();
		behaviorReq_ = std::nullopt;
		atkCollider_->isActive_ = false;
		//実際の初期化処理
		(this->*BehaviorInitialize[(int)behavior_])();
	}

	//状態の更新
	(this->*BehaviorUpdate[(int)behavior_])();

	GameObject::Update();
	shadow_->Update();

	atkCollider_->Update();
	collider_->Update();
}

void (ALBoss::* ALBoss::BehaviorInitialize[])() = {
	&ALBoss::InitIdle,
	&ALBoss::InitMove,
	&ALBoss::InitATK
};

void (ALBoss::* ALBoss::BehaviorUpdate[])() = {
	&ALBoss::UpdateIdle,
	&ALBoss::UpdateMove,
	&ALBoss::UpdateATK
};

void ALBoss::Draw()
{
	shadow_->Draw();
	GameObject::Draw();

	collider_->Draw();
	atkCollider_->Draw();
}

void ALBoss::OnCollision()
{
	HP_--;

	if (HP_-- <= 0) {

	}
}

Vector3 ALBoss::GetBoss2PlayerDirection()
{
	return player_->GetWorld().GetWorldTranslate() - world_.GetWorldTranslate();
}

#pragma region 各状態初期化と更新
void ALBoss::InitIdle() { idle_->Initialize(); }

void ALBoss::InitMove() { move_->Initialize(); }

void ALBoss::InitATK() { atk_->Initialize(); atkCollider_->isActive_ = true; }

void ALBoss::UpdateIdle() { idle_->Update(); }

void ALBoss::UpdateMove() { move_->Update(); }

void ALBoss::UpdateATK() { atk_->Update(); }


#pragma endregion


