#pragma once

#include<wrl.h>

#define DIRECTINPUT_VERSION 0x0800 //バージョン設定
#include<dinput.h>
#include <XInput.h>
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"WinApp/WinApp.h"

//パッド入力のサポ
enum kPadButton {
	kButtonA,
	kButtonB,
	kButtonX,
	kButtonY,
	kLeftTrigger,
	kRightTrigger,

	kUp,
	kDown,
	kLeft,
	kRight
};

//入力処理受付クラス
class Input {
public://**シングルトンパターン**//
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static Input* GetInstance();
private:
	Input() = default;
	~Input() = default;
	Input(const Input& o) = delete;
	const Input& operator=(const Input& o) = delete;

public:	//**パブリック関数**//

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WindowApp*winApp);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ボタン押し込む
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>押しているか否か</returns>
	bool PushKey(BYTE keyNum);

	/// <summary>
	/// トリガーチェック
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>押したか否か</returns>
	bool TriggerKey(BYTE keyNum);

	/// <summary>
	/// WASDをX、Zで受付
	/// </summary>
	/// <returns>X,Zに入れて返却</returns>
	Vector3 GetWASD();

	/// <summary>
	/// 矢印キー入力チェック
	/// </summary>
	/// <returns>X,Zに入れて返却</returns>
	Vector3 GetAllArrowKey();

	/// <summary>
	/// コントローラが接続されているか
	/// </summary>
	/// <returns>T/F</returns>
	bool IsControllerActive();

	/// <summary>
	/// 何かボタンを押したか否か
	/// </summary>
	/// <returns>T/F</returns>
	bool IsPushSomeButtons();

	/// <summary>
	/// ジョイスティック右取得
	/// </summary>
	/// <returns></returns>
	Vector2 GetjoyStickR();

	/// <summary>
	/// ジョイスティック左取得
	/// </summary>
	/// <returns></returns>
	Vector2 GetjoyStickL();

	/// <summary>
	/// yを0にしたxzで値を変換
	/// </summary>
	/// <returns></returns>
	Vector3 GetjoyStickLVec3();

	/// <summary>
	/// PADボタン押したか否か
	/// </summary>
	/// <param name="kButton">ボタン</param>
	/// <returns>T/F</returns>
	bool IsPushButton(kPadButton kButton);

	/// <summary>
	/// PADボタントリガーしたか
	/// </summary>
	/// <param name="kButton">ボタン</param>
	/// <returns>T/F</returns>
	bool IsTriggerButton(kPadButton kButton);

	/// <summary>
	/// デッドラインの設定
	/// </summary>
	/// <param name="deadLine">値</param>
	void SetDeadLine(const float deadLine) { deadLine_ = deadLine; }

private://**プライベート変数**//

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	//キーボードデバイス
	ComPtr<IDirectInputDevice8>keyboad;

	//キー入力
	BYTE key[256] = {};

	//1フレーム前キー入力
	BYTE preKey[256] = {};

	//コントローラー入力
	XINPUT_STATE joyState;

	//1フレーム前コントローラー入力
	XINPUT_STATE preJoyState_;

	//デッドライン
	float deadLine_ = 0.2f;
};