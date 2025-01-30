#include"Input.h"


#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

#include<cassert>

using namespace Engine;

Input* Input::GetInstance() {
	//インスタンス取得
	static Input instance;
	return &instance;
}

void Input::Initialize(WindowApp* winApp) {

	//インスタンス生成
	ComPtr<IDirectInput8>directInput = nullptr;
	HRESULT hr = DirectInput8Create(winApp->GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイス
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboad, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式リセット
	hr = keyboad->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboad->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update() {

	//キーの入力状態を過去に保存
	memcpy(preKey, key, sizeof(key));

	//コントローラ系列も
	preJoyState_ = joyState;

	//キーボード情報取得開始
	keyboad->Acquire();

	//状態を取得
	keyboad->GetDeviceState(sizeof(key), key);

	//コントローラ取得
	DWORD res = XInputGetState(0, &joyState);
	if (res != ERROR_SUCCESS) {
		//assert(false);
	}
}

bool Input::PushKey(BYTE keyNum)
{
	//有効の場合true
	if (key[keyNum]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNum) {
	//現在はTで過去がFの時true
	if (key[keyNum] && !preKey[keyNum]) {
		return true;
	}
	return false;
}

Vector3 Input::GetWASD() {
	//3DでのXZ平面想定での簡易移動入力取得処理

	//返却地
	Vector3 ans{};

	//フラグを取得
	if (key[DIK_W]) {
		ans.z += 1;
	}
	if (key[DIK_S]) {
		ans.z -= 1;
	}
	if (key[DIK_D]) {
		ans.x += 1;
	}
	if (key[DIK_A]) {
		ans.x -= 1;
	}

	//返却
	return ans;
}


Vector3 Input::GetAllArrowKey() {
	//3DでのXZ平面想定での簡易移動入力取得処理
	//返却する値をいれる変数
	Vector3 result;

	//入力チェック
	if (key[DIK_UP]) {
		result.z += 1;
	}
	if (key[DIK_DOWN]) {
		result.z -= 1;
	}
	if (key[DIK_RIGHT]) {
		result.x += 1;
	}
	if (key[DIK_LEFT]) {
		result.x -= 1;
	}

	//返却
	return result;

}

bool Input::IsControllerActive() {

	//コントローラーが有効かチェック
	if (XInputGetState(0, &joyState) == ERROR_SUCCESS) {
		return true;
	}
	return false;
}

bool Input::IsPushSomeButtons() {

	//どれかでも入力があれば帰す
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		return true;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		return true;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
		return true;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
		return true;
	}

	return false;
}

Vector2 Input::GetjoyStickR() {

	//入力方向を受け取る（０-１）
	Vector2 r = {
		(float)joyState.Gamepad.sThumbRX / SHRT_MAX,
		(float)joyState.Gamepad.sThumbRY / SHRT_MAX,
	};

	//デッドラインより少なければ０
	if (r.x > 0 && r.x < deadLine_) {
		r.x = 0;
	}
	if (r.x < 0 && r.x > -deadLine_) {
		r.x = 0;
	}

	if (r.y > 0 && r.y < deadLine_) {
		r.y = 0;
	}
	if (r.y < 0 && r.y > -deadLine_) {
		r.y = 0;
	}

	//帰す
	return r;
}

Vector2 Input::GetjoyStickL() {
	//入力方向を受け取る（０-１）
	Vector2 r = {
		(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		(float)joyState.Gamepad.sThumbLY / SHRT_MAX,
	};

	//デッドラインより少なければ０
	if (r.x > 0 && r.x < deadLine_) {
		r.x = 0;
	}
	else if (r.x < 0 && r.x > -deadLine_) {
		r.x = 0;
	}

	if (r.y > 0 && r.y < deadLine_) {
		r.y = 0;
	}
	else if (r.y < 0 && r.y > -deadLine_) {
		r.y = 0;
	}

	return r;
}

Vector3 Input::GetjoyStickLVec3() {
	//XZ平面のゲームで移動処理を容易にできる関数

	//入力がなければ０で返却
	if (!IsControllerActive()) {
		return { 0,0,0 };
	}

	//入力方向を受け取る（０-１）
	Vector3 r = {
		(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		0,
		(float)joyState.Gamepad.sThumbLY / SHRT_MAX,
	};

	//デッドラインより少なければ０
	if (r.x > 0 && r.x < deadLine_) {
		r.x = 0;
	}
	else if (r.x < 0 && r.x > -deadLine_) {
		r.x = 0;
	}

	if (r.z > 0 && r.z < deadLine_) {
		r.z = 0;
	}
	else if (r.z < 0 && r.z > -deadLine_) {
		r.z = 0;
	}

	//返却
	return r;
}

bool Input::IsPushButton(kPadButton kButton) {
	//ボタンを押しているかチェック
	//ボタンに合わせた処理に移行
	switch (kButton) {
	case kButtonA:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			return true;
		}
		break;
	case kButtonB:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			return true;
		}
		break;
	case kButtonX:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			return true;
		}
		break;
	case kButtonY:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
			return true;
		}
		break;
	case kLeftTrigger:
		if (joyState.Gamepad.bLeftTrigger) {
			return true;
		}
		break;
	case kRightTrigger:
		if (joyState.Gamepad.bRightTrigger) {
			return true;
		}
		break;

	case kUp:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
			return true;
		}
		break;
	case kDown:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
			return true;
		}
		break;
	case kLeft:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			return true;
		}
		break;
	case kRight:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			return true;
		}
		break;
	default:

		break;
	}

	//なかったのでfalse
	return false;
}

bool Input::IsTriggerButton( kPadButton kButton) {

	//コントローラーが無効の場合
	if (!IsControllerActive()) {
		//早期リターン
		return false;
	}

	//今押されているかチェック
	if (IsPushButton(kButton)) {

		//過去に押されていないかチェック
		switch (kButton) {
		case kButtonA:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				return false;
			}
			break;
		case kButtonB:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
				return false;
			}
			break;
		case kButtonX:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
				return false;
			}
			break;
		case kButtonY:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
				return false;
			}
			break;
		case kLeftTrigger:
			if (preJoyState_.Gamepad.bLeftTrigger) {
				return false;
			}
			break;
		case kRightTrigger:
			if (preJoyState_.Gamepad.bRightTrigger) {
				return false;
			}
			break;

		case kUp:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
				return false;
			}
			break;
		case kDown:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
				return false;
			}
			break;
		case kLeft:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
				return false;
			}
			break;
		case kRight:
			if (preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
				return false;
			}
			break;
		default:
			break;
		}

		return true;
	}
	return false;
}
