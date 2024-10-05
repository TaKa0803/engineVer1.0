#pragma once

class ALPlayer;

class PlayerPunch {

public://パブリック関数
	PlayerPunch(ALPlayer* player);
	~PlayerPunch() = default;

	void Initialize();
	void Update();

private://プライベート関数

	void CheckNextState();

public:

	//パブリック変数
	bool isEnd_ = false;

private://プライベート変数
	ALPlayer* player_;

	//最大攻撃回数
	const int maxAtkCount_ = 3;

private://パラメータ
	struct Com1
	{
		//開始交直
		float stStiffnessSec = 1.0f;
		//攻撃状態
		float atkSec = 1.0f;
		//攻撃終了交直
		float edStiffnessSec = 1.0f;
	};

	struct Com2
	{
		//開始交直
		float stStiffnessSec = 1.0f;
		//攻撃状態
		float atkSec = 1.0f;
		//攻撃終了交直
		float edStiffnessSec = 1.0f;
	};

	struct Com3
	{
		//開始交直
		float stStiffnessSec = 1.0f;
		//攻撃状態
		float atkSec = 1.0f;
		//攻撃終了交直
		float edStiffnessSec = 1.0f;
	};

	struct Parameters {
		//攻撃のコンボ数
		int atkCount_ = 0;
		//各カウント
		float count = 0;
		//状態前初期化処理
		bool isInit = false;

		//次の攻撃の有効処理
		bool isNextInput = false;


		//各攻撃のパラメータ
		Com1 com1 = Com1{};
		Com2 com2 = Com2{};
		Com3 com3 = Com3{};
	};

	//攻撃の状態取得
	enum AtkState {
		St,
		Atk,
		Ed,
		CheckEnd,
		_countAtkState
	};
	AtkState state_=St;

	Parameters parameters_ = Parameters{};
};