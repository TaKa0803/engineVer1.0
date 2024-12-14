#pragma once
#include"GvariGroup/GvariGroup.h"

//プレイヤー前方宣言
class Player;

//プレイヤーパンチ
class PlayerPunch {

public://**パブリック変数**//

	//終了フラグ
	bool isEnd_ = false;

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player"></param>
	PlayerPunch(Player* player);
	~PlayerPunch() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ツリー取得
	/// </summary>
	/// <returns></returns>
	GlobalVariableTree& GetTree() { return tree_; }

private://**プライベート関数**//

	/// <summary>
	/// 各状態での処理の変更
	/// </summary>
	void CheckNextState();

	/// <summary>
	/// 各状態でのアニメーション変更処理
	/// </summary>
	void ChangeAnimation();



private://**プライベート変数**//

	//プレイヤーポインタ
	Player* player_;

	//最大攻撃回数
	const int maxAtkCount_ = 3;

private://パラメータ

	struct Com1
	{
		//移動速度倍率
		float multiSpd = 0.1f;
		//開始交直
		float stStiffnessSec = 0.2f;
		//攻撃状態
		float atkSec = 0.2f;
		//攻撃終了交直
		float edStiffnessSec = 0.2f;
	};

	struct Com2
	{
		//移動速度倍率
		float multiSpd = 0.1f;
		//開始交直
		float stStiffnessSec = 0.3f;
		//攻撃状態
		float atkSec = 0.2f;
		//攻撃終了交直
		float edStiffnessSec = 0.3f;
	};

	struct Com3
	{
		//移動速度倍率
		float multiSpd = 0.1f;
		//開始交直
		float stStiffnessSec = 0.4f;
		//攻撃状態
		float atkSec = 0.3f;
		//攻撃終了交直
		float edStiffnessSec = 0.4f;
	};

	struct Parameters {
		//攻撃のコンボ数
		int atkCount_ = 0;
		//各カウント
		float count = 0;

		//現在のコンボでの各時間
		float multiSpd;
		float stStiffness;
		float atk;
		float edStiffness;

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
	}state_=St;

	//パラメータ変数
	Parameters parameters_ = Parameters{};

	//音のデータポインタ
	//パンチ
	int punchSound_;
	//キック
	int kickSound_;
	//ドリル
	int drilSound_;


private:

	//ツリー
	GlobalVariableTree tree_ = GlobalVariableTree("");

};