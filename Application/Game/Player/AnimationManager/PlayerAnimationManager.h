#pragma once
#include<string>

//プレイヤー前方宣言
class Player;

//プレイヤーのアニメーション関係クラス
class PlayerAnimationManager {

public://**パブリック変数**//

	//アニメーションの種類
	enum  AnimationData {
		Idle,	//待機
		Moving,	//移動
		Dash,	//走る

		Roll,	//回転
		RollEnd,//回転終わり

		PrePunch1,//パンチ１構え
		PrePunch2,//パンチ２構え
		PrePunch3,//パンチ３構え
		Punch1,//パンチ1
		Punch2,//パンチ2
		Punch3,//パンチ3

		Dawn,//ダウン
		DawnBack,//ダウン復帰
		CountAnime//アニメーションの数
	};

public://**パブリック関数**//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player"></param>
	PlayerAnimationManager(Player*player);
	~PlayerAnimationManager()=default;

	/// <summary>
	/// アニメーションのセット
	/// </summary>
	/// <param name="data">アニメ名</param>
	/// <param name="sec">補完時間</param>
	/// <param name="loopSecMulti">ループ再生倍率</param>
	/// /// <param name="loop">ループ再生フラグ</param>
	void SetAnimation(AnimationData data, float sec,  float loopSecMulti=1.0f,bool loop = true);

	/// <summary>
	/// アニメーション再生進行度変更
	/// </summary>
	/// <param name="active">処理のフラグ</param>
	/// <param name="t"></param>
	void SetAnimeTime(bool active, float t = 0);


private://**プライベート変数**//

	//プレイヤーのポインタ
	Player* player_=nullptr;

	//アニメーション名まとめ
	std::string animeName_[CountAnime] = {
		"Idle",
		"walk",
		"Dash",

		"Rolling",
		"RollingEnd",

		"PrePunch",
		"PrePunch2",
		"PrePunch3",
		"Punch",
		"Punch2",
		"Punch3",

		"Down",
		"DownBack",
	};

};


//プレイヤーアニメーションマネージャの省略名
using PAnimeM = PlayerAnimationManager;