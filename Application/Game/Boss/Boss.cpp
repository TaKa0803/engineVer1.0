#include "Boss.h"
#include"ImGuiManager/ImGuiManager.h"
#include"Game/Boss//ATK/IATK/IATK.h"
#include"GvariGroup/GvariGroup.h"

#include<numbers>

Boss::Boss(Player* player)
{
	//プレイヤーポインタ取得
	player_ = player;

	//一回しかしない初期化情報
	input_ = Input::GetInstance();
	input_->SetDeadLine(0.3f);

	//弾マネージャのインスタンス取得
	bulletM_ = BossBulletManager::GetInstance();
	bulletM_->SetUp();

	GameObject::Initialize("Boss");
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);

	model_->IsEnableTexture(false);
	model_->SetColor({ 1,1,1,1 });


	collider_ = std::make_unique<SphereCollider>();
	collider_->Initialize("Boss", world_);
	collider_->SetRadius(2.0f);
	collider_->SetTranslate({ 0,1.4f,0 });

	shadow_ = std::make_unique<CirccleShadow>(world_);

	idle_ = std::make_unique<BossIdle>(this);
	move_ = std::make_unique<BossMove>(this);
	atk_ = std::make_unique<BossATKTypeManager>(this);


	atkCollider_ = std::make_unique<SphereCollider>();
	atkCollider_->Initialize("boss", world_);
	atkCollider_->SetRadius(1.0f);
	atkCollider_->SetTranslate({ 0.0f,1.4f,0.0f });
	atkCollider_->isActive_ = false;

	//Gvari設定
	std::unique_ptr<GVariGroup> gvg = std::make_unique<GVariGroup>("Boss");
	gvg->SetMonitorValue("behavior",&nowBehaviorName_);
	gvg->SetValue("HP", &HP_);

	gvg->SetTreeData(idle_->GetTree());
	gvg->SetTreeData(move_->GetTree());


}




void Boss::Init()
{
	bulletM_->Init();

	world_.Initialize();
	world_.translate_ = { 0,0,10 };
	world_.scale_ = { 1,1,1 };
	shadow_->SetShadowScale(GetAllScaleX(world_));
	atk_->SceneInit();
	isDead_ = false;

	behaviorReq_ = Behavior::IDLE;
}

void Boss::Update()
{
#ifdef _DEBUG

	nowBehaviorName_ = behaviorName_[(int)behavior_];

	model_->DebugParameter("Boss");
	collider_->Debug("BossCollider");
	atkCollider_->Debug("BossATK");
#endif // _DEBUG


	//状態の初期化処理
	if (behaviorReq_) {
		behavior_ = behaviorReq_.value();
		behaviorReq_ = std::nullopt;
		atkCollider_->isActive_ = false;
		SetAnimeTime(false);
		//実際の初期化処理
		(this->*BehaviorInitialize[(int)behavior_])();
	}

	//状態の更新
	(this->*BehaviorUpdate[(int)behavior_])();

	bulletM_->Update();
	GameObject::Update();
	shadow_->Update();

	atkCollider_->Update();
	collider_->Update();
}

void (Boss::* Boss::BehaviorInitialize[])() = {
	&Boss::InitIdle,
	&Boss::InitMove,
	&Boss::InitATK
};

void (Boss::* Boss::BehaviorUpdate[])() = {
	&Boss::UpdateIdle,
	&Boss::UpdateMove,
	&Boss::UpdateATK
};

void Boss::Draw()
{
	shadow_->Draw();
	GameObject::Draw();

	bulletM_->Draw();

	collider_->Draw();
	atkCollider_->Draw();
}

void Boss::OnCollision()
{
	HP_--;
	if (HP_ <= 0) {
		isDead_ = true;
	}

}

Vector3 Boss::GetBoss2PlayerDirection()
{
	return player_->GetWorld().GetWorldTranslate() - world_.GetWorldTranslate();
}

void Boss::SetDirection2Player()
{
	//プレイヤー方向を見続ける
	Vector3 direc = GetBoss2PlayerDirection();

	if (direc != Vector3(0, 0, 0)) {
		world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}
}

void Boss::SetAnimation(int animeNum, float sec, float loopSec, bool isLoop)
{
	model_->ChangeAnimation(animeNum, sec);
	model_->SetAnimationRoop(isLoop);
	model_->animationRoopSecond_ = loopSec;
}

#pragma region 各状態初期化と更新

void Boss::InitIdle() { idle_->Initialize(); }

void Boss::InitMove() { move_->Initialize(); }

void Boss::InitATK() { atk_->Initialize(); atkCollider_->isActive_ = true; }

void Boss::UpdateIdle() { idle_->Update(); }

void Boss::UpdateMove() { move_->Update(); }

void Boss::UpdateATK() { atk_->Update(); }


#pragma endregion


