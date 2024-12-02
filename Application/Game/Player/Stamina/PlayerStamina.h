#pragma once
#include"GvariGroup/GvariGroup.h"
#include"Sprite/Sprite.h"

//前方宣言
class Player;

class PlayerStamina {

public:
	//動きのタイプ
	enum class Type {
		DASH,
		ROLL,
		ATK,
		HIT,
		CountType
	};

public:
	//コンストラクタ
	PlayerStamina();
	~PlayerStamina()=default;

	//初期化
	void Init();

	//更新
	void Update();

	//gage
	void DrawGage();

	/// <summary>
	/// 選択したタイプがスタミナたりるかチェック
	/// </summary>
	/// <param name="type">スタミナを消費する行動タイプ</param>
	/// <returns>スタミナがたりればTrue</returns>
	bool CheckStamina(Type type);

	/// <summary>
	/// 指定した動きのスタミナを消費
	/// </summary>
	/// <param name="type"></param>
	void UseStamina(Type type);

	//ツリー取得
	GVariTree& GetTree() { return tree_; }
private:
	
	//スタミナゲージ
	std::unique_ptr<Sprite>gage_;

	//赤ゲージ
	std::unique_ptr<Sprite>redGage_;

private://パラメータ

	//スタミナデータ
	struct StaminaData {
		
		
		//スタミナ関係
		float maxStamina = 100;
		float currentStamina = StaminaData::maxStamina;

		//回復開始カウント
		float rechargeSec = 1.0f;
		float currentCharge = 0.0f;

		//回復速度
		float healSec = 100;

		//各アクションのスタミナコスト
		float rollCost = 20;

		float dashCostSec = 10;

		float atkCost = 10;

		float hitCost = 30;
	}data_{};

	//最大スケール
	float maxScaleX_=128;

	//揺れフラグ
	bool isShake_ = false;
	//シェイクする時間
	float shakeSec_ = 1.0f;
	float currentShake_ = 0;
	//揺れ幅
	float diffX_ = 10.0f;

	//画像の元の位置
	Vector3 basePos_{};

	//スタミナ量での色
	Vector4 maxColor_{1,1,1,1};
	Vector4 minColor_{1,1,1,1};

	//赤ゲージ処理フラグ
	bool isRedgage_ = false;
	//ゲージが消える時間
	float fadeOutRedGageSec_ = 5.0f;
	//カウント
	float currentRedGage_ = 0;
	//減った現在のTの値
	float nowT_ = 0;
	float preT_ = 0;

private://デバッグ関係
	GVariTree tree_ = GVariTree("スタミナ");

};