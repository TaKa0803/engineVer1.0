#include "Boss.h"
#include"ImGuiManager/ImGuiManager.h"
#include"GVariableManager/GVaribleManager.h"

#include"Game/Boss/Behavior/Idle/BossIdle.h"
#include"Game/Boss/Behavior/Down/BossDown.h"
#include"Game/Boss/Behavior/ATK/BossATKTypeManager.h"

#include<numbers>

Boss::Boss(Player* player)
{
	//プレイヤーポインタ取得
	player_ = player;

	//入力インスタンス取得
	input_ = Input::GetInstance();
	//デッドライン設定
	input_->SetDeadLine(0.3f);

	//弾マネージャのインスタンス取得
	bulletM_ = BossBulletManager::GetInstance();
	//初期設定
	bulletM_->SetUp();

	stumpEffect_ = std::make_unique<EffectLargeDebris>();

	//オブジェクト初期化
	GameObject::Initialize("Boss");
	//アニメーションを有効化
	model_->SetAnimationActive(true);
	model_->SetAnimeSecond(10);

	//画像を無効
	model_->IsEnableTexture(false);
	//色をセット
	model_->SetColor({ 0.8f,0,0,1 });

	//コライダー初期化
	collider_ = std::make_unique<SphereCollider>();
	//追従先を指定して初期化
	collider_->Initialize("Boss", world_);

	//円影生成
	shadow_ = std::make_unique<CirccleShadow>(world_);

	//iiUI生成
	ui_ = std::make_unique<BossUI>(this);

	//状態の量読み込み
	behaviors_.resize((size_t)Behavior::CountBehavior);

	behaviors_[(int)Behavior::IDLE] = std::make_unique<BossIdle>(this);
	behaviors_[(int)Behavior::DOWN] = std::make_unique<BossDown>(this);
	behaviors_[(int)Behavior::ATK] = std::make_unique<BossATKTypeManager>(this);


	//攻撃コライダー生成
	atkCollider_ = std::make_unique<SphereCollider>();
	//ターゲット指定して初期化
	atkCollider_->Initialize("boss", world_);
	//コライダーを無効
	atkCollider_->isActive_ = false;

	//Gvari設定
	std::unique_ptr<GVariGroup> gvg = std::make_unique<GVariGroup>(groupName_);
	gvg->SetMonitorValue("思考", &brein_);
	gvg->SetMonitorValue("当たり判定", &hit_);
	gvg->SetMonitorValue("behavior", &nowBehaviorName_);
	gvg->SetMonitorValue("現在の体力", &HP_);
	gvg->SetValue("MAXHP", &maxHP_);
	gvg->SetValue("ボスサイズ", &world_.scale_);
	gvg->SetTreeData(collider_->GetDebugTree("体コライダー"));
	gvg->SetTreeData(atkCollider_->GetDebugTree("攻撃コライダー"));

	gvg->SetTreeData(behaviors_[(int)Behavior::IDLE]->GetTree());
	gvg->SetTreeData(behaviors_[(int)Behavior::DOWN]->GetTree());
	gvg->SetTreeData(behaviors_[(int)Behavior::ATK]->GetTree());

	gvg->SetTreeData(ui_->GetTree());

	gvg->SetTreeData(model_->SetDebugParam("model"));
	gvg->SetTreeData(stumpEffect_->GetDebugTree("ra"));
	gvg->SetTreeData(bulletM_->GetTree());

}




void Boss::Init()
{
	//弾マネージャ初期化
	bulletM_->Init();

	//座標初期化
	world_.Initialize();
	//初期座標に設定
	world_.translate_ = { 0,0,10 };
	//サイズ
	world_.scale_ = { 1,1,1 };
	shadow_->SetShadowScale(GetAllScaleX(world_));
	isDead_ = false;

	//状態初期化
	behaviorReq_ = Behavior::IDLE;

	//HPのセット
	HP_ = maxHP_;

	//セットしたパラメータのセーブデータ読み込み
	GlobalVariableManager::GetInstance()->SetLoadGroupData(groupName_);

	//UI初期化
	ui_->Initialize();
}

void Boss::Update()
{
#ifdef _DEBUG

	nowBehaviorName_ = behaviorName_[(int)behavior_];

#endif // _DEBUG

	//死亡チェック
	if (behavior_ != Behavior::DOWN) {
		if (HP_ <= 0) {
			behaviorReq_ = Behavior::DOWN;
		}
	}

	if (brein_) {
		//状態の初期化処理
		if (behaviorReq_) {
			//リクエスト状態を渡す
			behavior_ = behaviorReq_.value();
			//渡したので空にする
			behaviorReq_ = std::nullopt;
			//攻撃コライダーをOFFにする
			atkCollider_->isActive_ = false;
			//アニメのゲーム側管理をOFF
			SetAnimeTime(false);
			//実際の初期化処理
			behaviors_[(int)behavior_]->Initialize();
		}

		//状態の更新
		behaviors_[(int)behavior_]->Update();

	}

	bulletM_->Update();
	GameObject::Update();
	shadow_->Update();

	stumpEffect_->Update();
	ui_->Update();

	atkCollider_->Update();
	collider_->Update();
}

void Boss::Draw()
{
	shadow_->Draw();
	GameObject::Draw();

	bulletM_->Draw();

	stumpEffect_->Draw();

	collider_->Draw();
	atkCollider_->Draw();
}

void Boss::DrawUI()
{
	ui_->Draw();
}

void Boss::OnCollision()
{

	if (hit_) {

		HP_--;
		if (HP_ <= 0) {
			//isDead_ = true;
		}

	}
}

Vector3 Boss::GetBoss2PlayerDirection()
{
	return player_->GetWorld().GetWorldTranslate() - world_.GetWorldTranslate();
}

void Boss::SetNextATK(int atkNum)
{
	//攻撃を指定
	behaviorReq_ = Behavior::ATK;
	//攻撃マネージャに番号を送信
	BossATKTypeManager* atkM = dynamic_cast<BossATKTypeManager*>(behaviors_[(int)Behavior::ATK].get());
	if (atkM) {
		atkM->GetATKContract(atkNum);
	}
}

void Boss::SetDirection2Player()
{
	//プレイヤー方向を見続ける
	Vector3 direc = GetBoss2PlayerDirection();

	if (direc != Vector3(0, 0, 0)) {
		world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}
}

void Boss::SetAnimation(const std::string& animeName, float sec, float loopSec, bool isLoop)
{
	model_->ChangeAnimation(animeName, sec);
	model_->SetAnimationRoop(isLoop);
	model_->animationRoopSecond_ = loopSec;
}




