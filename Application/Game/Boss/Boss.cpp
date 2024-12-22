#include "Boss.h"
#include"ImGuiManager/ImGuiManager.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"
#include"GlobalVariable/Manager/GlobalVaribleManager.h"

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
	bulletM_ = std::make_unique<BossBulletManager>();

	//落下エフェクト生成
	stumpEffect_ = std::make_unique<EffectLargeDebris>();

	//オブジェクト初期化
	GameObject::Initialize("Boss");
	//アニメーションを有効化
	model_->SetAnimationActive(true);

	//体コライダー初期化
	bodyCollider_ = std::make_unique<SphereCollider>();
	//追従先を指定して初期化
	bodyCollider_->Initialize("Boss", world_);

	//攻撃コライダー生成
	atkCollider_ = std::make_unique<SphereCollider>();
	//ターゲット指定して初期化
	atkCollider_->Initialize("boss", world_);

	//円影生成
	shadow_ = std::make_unique<CirccleShadow>(world_);

	//iiUI生成
	ui_ = std::make_unique<BossUI>(this);

	//状態の量読み込み
	behaviors_.resize((size_t)Behavior::CountBehavior);

	//各状態の生成
	behaviors_[(int)Behavior::IDLE] = std::make_unique<BossIdle>(this);
	behaviors_[(int)Behavior::DOWN] = std::make_unique<BossDown>(this);
	behaviors_[(int)Behavior::ATK] = std::make_unique<BossATKTypeManager>(this);

	//デバッグ用にパラメータを設定
	//パラメータグループを作成
	std::unique_ptr<GlobalVariableGroup> gvg = std::make_unique<GlobalVariableGroup>(groupName_);
	gvg->SetMonitorValue("思考", &brein_);
	gvg->SetMonitorValue("当たり判定", &hit_);
	gvg->SetMonitorValue("behavior", &nowBehaviorName_);
	gvg->SetMonitorValue("現在の体力", &nowHP);
	gvg->SetValue("MAXHP", &maxHP_);
	gvg->SetValue("ボスサイズ", &world_.scale_);
	//グループにデバッグツリーを設定
	gvg->SetTreeData(bodyCollider_->GetDebugTree("体コライダー"));
	gvg->SetTreeData(atkCollider_->GetDebugTree("攻撃コライダー"));
	for (auto& behav : behaviors_) {
		gvg->SetTreeData(behav->GetTree());
	}
	gvg->SetTreeData(ui_->GetTree());
	gvg->SetTreeData(model_->SetDebugParam("model"));
	gvg->SetTreeData(stumpEffect_->GetDebugTree("ra"));
	gvg->SetTreeData(bulletM_->GetTree());

}




void Boss::Initialize()
{
	//弾マネージャ初期化
	bulletM_->Initialize();

	//座標初期化
	world_.Initialize();
	//初期座標に設定
	world_.translate_ = { 0,0,10 };
	//サイズ
	world_.scale_ = { 1,1,1 };
	//円影のサイズ設定
	shadow_->SetShadowScale(GetAllScaleX(world_));
	//死亡フラグを切る
	isDead_ = false;

	//アニメーションセット
	SetAnimation(animeName_[(int)Boss::Animation::Idle2], 0,1.0f);

	//状態初期化
	behaviorReq_ = Behavior::IDLE;

	//HPのセット
	nowHP = maxHP_;

	//コライダーを無効
	atkCollider_->isActive_ = false;

	//セットしたパラメータのセーブデータ読み込み
	//GlobalVariableManager::GetInstance()->SetLoadGroupData(groupName_);

	//UI初期化
	ui_->Initialize();
}

void Boss::Update()
{
#ifdef _DEBUG
	//現在の状態名を取得
	nowBehaviorName_ = behaviorName_[(int)behavior_];
#endif // _DEBUG

	//死亡チェック
	if (behavior_ != Behavior::DOWN) {
		//HP０でダウン
		if (nowHP <= 0) {
			//ダウン状態に移行
			behaviorReq_ = Behavior::DOWN;
		}
	}

	//思考処理フラグがある場合
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

	//弾マネージャ更新
	bulletM_->Update();

	//オブジェクトの更新
	GameObject::Update();

	//円影の更新
	shadow_->Update();

	//落下攻撃エフェクトの更新
	stumpEffect_->Update();

	//UIの更新
	ui_->Update();

	//コライダーの更新
	atkCollider_->Update();
	bodyCollider_->Update();
}

void Boss::Draw()
{
	//円影描画
	shadow_->Draw();
	
	//モデルの描画
	GameObject::Draw();

	//弾の描画
	bulletM_->Draw();

	//落下攻撃のエフェクト描画
	stumpEffect_->Draw();

	//コライダー描画
	bodyCollider_->Draw();
	atkCollider_->Draw();
}

void Boss::DrawUI()
{
	//UIの描画
	ui_->Draw();
}

void Boss::OnCollision()
{
	//ヒットフラグがある場合
	if (hit_) {
		//HPを減らす
		nowHP--;
	}
}

bool Boss::CheckCollisionAmmo(const SphereCollider* co)
{
	return bulletM_->CheckCollision(co);
}

Vector3 Boss::GetBoss2PlayerDirection()
{
	//ボスからプレイヤーへの向きベクトルを返却
	return player_->world_.GetWorldTranslate() - world_.GetWorldTranslate();
}

void Boss::SetNextATK(int atkNum)
{
	//攻撃を指定
	behaviorReq_ = Behavior::ATK;
	//攻撃マネージャに番号を送信
	BossATKTypeManager* atkM = dynamic_cast<BossATKTypeManager*>(behaviors_[(int)Behavior::ATK].get());
	if (atkM) {
		//番号の設定
		atkM->GetATKContract(atkNum);
	}
}

void Boss::SetDirection2Player()
{
	//プレイヤー方向取得
	Vector3 direc = GetBoss2PlayerDirection();

	//重なっていない場合
	if (direc != Vector3(0, 0, 0)) {
		//回転量Yを求め返却
		world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}
}

void Boss::SetAnimation(const std::string& animeName, float sec, float loopSec, bool isLoop)
{
	//アニメーションと遷移時間を設定
	model_->ChangeAnimation(animeName, sec);
	//ループフラグ設定
	model_->SetAnimationRoop(isLoop);
	//アニメーション進行倍数設定
	model_->animationRoopSecond_ = loopSec;
}




