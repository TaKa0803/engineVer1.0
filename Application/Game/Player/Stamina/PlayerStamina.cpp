#include "PlayerStamina.h"
#include"DeltaTimer/DeltaTimer.h"
#include"TextureManager/TextureManager.h"

PlayerStamina::PlayerStamina()
{

	gage_.reset(Sprite::Create(TextureManager::white_, { 1,1 }, { 1,1 }, { 64,64 }, { 640,360 }, { 0.5f,1.0f }));
	


	tree_.SetMonitorValue("現スタミナ", &data_.currentStamina);
	tree_.SetMonitorValue("回復開始カウント", &data_.currentCharge);

	tree_.SetValue("最大スタミナ", &data_.maxStamina);
	tree_.SetValue("回復開始までの時間", &data_.rechargeSec);
	tree_.SetValue("一秒間の回復量", &data_.healSec);
	tree_.SetValue("回転時の消費量", &data_.rollCost);
	tree_.SetValue("ダッシュ時の消費量/1s", &data_.dashCostSec);
	tree_.SetValue("攻撃時の消費量", &data_.atkCost);
	tree_.SetValue("画像の最大サイズY", &maxScaleY_);
	tree_.SetTreeData(gage_->GetTree("ゲージ"));
}

void PlayerStamina::Init()
{
	data_.currentStamina = data_.maxStamina;
	data_.currentCharge = 0;
}

void PlayerStamina::Update()
{
	//チャージ開始までの時間カウント
	data_.currentCharge += (float)DeltaTimer::deltaTime_;

	//待機カウント以上で回復処理
	if (data_.currentCharge >= data_.rechargeSec) {
		data_.currentCharge = data_.rechargeSec;

		//回復処理
		data_.currentStamina += data_.healSec * (float)DeltaTimer::deltaTime_;
		//オーバーヒール処理
		if (data_.currentStamina > data_.maxStamina) {
			data_.currentStamina = data_.maxStamina;

			//zouka
			currentAlpha_ -= (float)DeltaTimer::deltaTime_;
		}
	}

	//画像の更新
	//割合取得
	float t = data_.currentStamina / data_.maxStamina;
	float scale = Lerp(0, maxScaleY_, t);
	Vector3 s= gage_->GetScale();
	s.y = scale;
	gage_->SetScale(s);

	//float alpha = currentAlpha_ / ;
}

void PlayerStamina::DrawGage()
{
	gage_->Draw();
}

bool PlayerStamina::CheckStamina(Type type)
{
	//各タイプでスタミナが充分かチェック
	switch (type)
	{
	case PlayerStamina::Type::DASH://ダッシュ処理

		//スタミナが足りる場合
		if (data_.currentStamina >= data_.dashCostSec * (float)DeltaTimer::deltaTime_) {
			//可能だと返却
			return true;
		}
		break;

	case PlayerStamina::Type::ROLL://ローリング処理
		//攻撃に必要なスタミナの値がある時
		if (data_.currentStamina >= data_.rollCost) {
			return true;
		}
		break;

	case PlayerStamina::Type::ATK://攻撃処理
		//ローリングに必要なスタミナの値がある時
		if (data_.currentStamina >= data_.atkCost) {
			//可能だと返却
			return true;
		}
		break;

	case PlayerStamina::Type::HIT://攻撃処理
		//ローリングに必要なスタミナの値がある時
		if (data_.currentStamina >= data_.hitCost) {
			//可能だと返却
			return true;
		}
		break;
	case PlayerStamina::Type::CountType:
		break;
	default:
		break;
	}

	//処理をおこなえなかったと返却
	return false;
}

void PlayerStamina::UseStamina(Type type)
{
	switch (type)
	{
	case PlayerStamina::Type::DASH:
		//スタミナを消費
		data_.currentStamina -= data_.dashCostSec * (float)DeltaTimer::deltaTime_;
		break;

	case PlayerStamina::Type::ROLL:
		//コスト分スタミナを消費
		data_.currentStamina -= data_.rollCost;
		break;

	case PlayerStamina::Type::ATK:
		//コスト分スタミナを消費
		data_.currentStamina -= data_.atkCost;
		break;

	case PlayerStamina::Type::HIT:
		//コスト分スタミナを消費
		data_.currentStamina -= data_.hitCost;
		break;
	case PlayerStamina::Type::CountType:
		break;
	default:
		break;
	}

	//スタミナの回復開始のカウントをリセット
	data_.currentCharge = 0;
}

