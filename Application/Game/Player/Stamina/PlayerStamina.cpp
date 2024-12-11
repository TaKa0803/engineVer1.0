#include "PlayerStamina.h"
#include"DeltaTimer/DeltaTimer.h"
#include"TextureManager/TextureManager.h"
#include"RandomNum/RandomNum.h"

PlayerStamina::PlayerStamina()
{

	gage_.reset(Sprite::Create(TextureManager::white_, { 1,1 }, { 1,1 }, { 64,64 }, { 640,360 }, { 0.0f,0.5f }));
	redGage_.reset(Sprite::Create(TextureManager::white_, { 1,1 }, { 1,1 }, { 64,64 }, { 640,360 }, { 0.0f,0.5f }));



	tree_.SetMonitorValue("現スタミナ", &data_.currentStamina);
	tree_.SetMonitorValue("回復開始カウント", &data_.currentCharge);

	tree_.SetValue("最大スタミナ", &data_.maxStamina);
	tree_.SetValue("回復開始までの時間", &data_.rechargeSec);
	tree_.SetValue("一秒間の回復量", &data_.healSec);
	tree_.SetValue("回転時の消費量", &data_.rollCost);
	tree_.SetValue("ダッシュ時の消費量/1s", &data_.dashCostSec);
	tree_.SetValue("攻撃時の消費量", &data_.atkCost);
	tree_.SetValue("画像の最大サイズX", &maxScaleX_);
	tree_.SetValue("最大HPの時の色", &maxColor_);
	tree_.SetValue("最小HPの時の色", &minColor_);
	tree_.SetValue("揺れる時間", &shakeSec_);
	tree_.SetValue("揺れる幅", &diffX_);
	tree_.SetTreeData(gage_->GetTree("ゲージ"));
	tree_.SetTreeData(redGage_->GetTree("赤ゲージ"));

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
		}
	}

	//画像の更新
	//割合取得
	float staminaT = data_.currentStamina / data_.maxStamina;
	float scale = Lerp(0, maxScaleX_, staminaT);
	Vector3 scaleVec3 = gage_->GetScale();
	scaleVec3.x = scale;
	gage_->SetScale(scaleVec3);
	gage_->SetMaterialDataColor(Lerp(minColor_, maxColor_, staminaT));

	//赤ゲージの進行処理
	if (isRedgage_) {
		if (currentRedGage_ >= fadeOutRedGageSec_) {
			isRedgage_ = false;
		}
		else {
			//カウント進行
			currentRedGage_ += (float)DeltaTimer::deltaTime_;
			//進行度
			float t = currentRedGage_ / fadeOutRedGageSec_;
			//遷移量変更
			float transition = Lerp(preT_, nowT_,t);
			//計算した遷移量でサイズ量を求める
			float scaleRedGage = Lerp(0, maxScaleX_, transition);
			//求めたものを渡す
			Vector3 scaleRedGageV3 = redGage_->GetScale();
			scaleRedGageV3.x = scaleRedGage;
			redGage_->SetScale(scaleRedGageV3);
		}
	}

	//ゲージの揺れ処理
	if (isShake_) {

		//カウント以上で終了
		if (currentShake_ >= shakeSec_) {
			//フラグを切る
			isShake_ = false;
			//座標を戻す
			gage_->SetPosition(basePos_);
		}
		else {
			currentShake_ += (float)DeltaTimer::deltaTime_;

			//左右のずれ取得
			float diffX = RandomNumber::Get(-diffX_, diffX_);

			//ずれた分の値取得
			Vector3 newP = basePos_;
			newP.x += diffX;
			//座標をセット
			gage_->SetPosition(newP);
		}
	}
}

void PlayerStamina::DrawGage()
{
	if (isRedgage_) {
		redGage_->Draw();
	}

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

	//シェイクフラグ有効
	if (!isShake_) {
		isShake_ = true;
		//シェイクカウントリセット
		currentShake_ = 0;

		//元座標保存
		basePos_ = gage_->GetPosition();
	}
	else {
		//
		//シェイクカウントリセット
		currentShake_ = 0;
	}
	//処理をおこなえなかったと返却
	return false;
}

void PlayerStamina::UseStamina(Type type)
{

	preT_ = data_.currentStamina / data_.maxStamina;;

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

	isRedgage_ = true;
	currentRedGage_ = 0;
	nowT_ = data_.currentStamina / data_.maxStamina;


}

