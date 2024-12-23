#include "PlayerInputManager.h"

PlayerInputManager::PlayerInputManager()
{
	//インスタンス取得
	input_ = Input::GetInstance();
	//デッドライン設定
	input_->SetDeadLine(0.3f);
}

bool PlayerInputManager::CheckInput(InputType type)
{
	//入力受け取りフラグ生成
	bool ans = false;
	//移動入力取得時に利用する変数
	Vector3 move;

	//入力タイプによって変更
	switch (type)
	{
	case PlayerInputManager::MOVE:				//移動入力フラグチェック
		//入力方向をチェック
		move = input_->GetWASD().SetNormalize();
		//コントローラーがあるならその値を追加
		if (input_->IsControllerActive()) {
			move += input_->GetjoyStickLVec3();
		}
		//入力が行われていない場合
		if (move == Vector3{ 0,0,0 }) {
			//無効
			ans = false;
		}
		else {
			//有効
			ans = true;
		}
		break;

	case PlayerInputManager::DASH:				//ダッシュ入力フラグチェック
		//キー入力チェック
		if (input_->PushKey(DIK_LSHIFT)) {
			ans = true;
		}
		//コントローラーチェック
		if (input_->IsPushButton(kButtonX)) {
			ans = true;
		}
		break;

	case PlayerInputManager::ROLL:				//回転入力フラグチェック
		//キー入力を取得
		if (input_->TriggerKey(DIK_SPACE)) {
			ans = true;
		}
		//コントローラーでの入力を取得
		if (input_->IsTriggerButton(kButtonA)) {
			ans = true;
		}
		break;

	case PlayerInputManager::ATK:				//攻撃入力チェック

		//キー入力でおこなわれていたか取得
		if (input_->TriggerKey(DIK_Z)) {
			ans = true;
		 }
		//コントローラーがあるときの処理
		if (input_->IsTriggerButton(kButtonB)) {
			ans = true;
		}
		break;

	case PlayerInputManager::CountInputType:
		break;
	default:
		break;
	}

	//返却
	return ans;
}

Vector3 PlayerInputManager::GetMoveVelo()
{
	//入力方向をチェック
	Vector3 move = input_->GetWASD().SetNormalize();

	//ローラーがあるならその値を追加
	move += input_->GetjoyStickLVec3();
	
	//正規化処理
	move.SetNormalize();

	return move;
}
