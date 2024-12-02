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
	float maxScaleY_=128;

	//
	float alphaSec = 1.0f;
	float currentAlpha_ = 0;

private://デバッグ関係
	GVariTree tree_ = GVariTree("スタミナ");

};