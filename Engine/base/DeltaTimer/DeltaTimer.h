#pragma once
#include<Windows.h>


//デルタタイム取得クラス
class DeltaTimer {

public: //**シングルトンパターン**//

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static DeltaTimer* GetInstance();

private: //**シングルトンパターン**//
	DeltaTimer() = default;
	~DeltaTimer() = default;
	DeltaTimer(const DeltaTimer& o) = delete;
	const DeltaTimer& operator=(const DeltaTimer& o) = delete;

public: //**静的変数**//

	//デルタタイム
	static double deltaTime_;

public: //**パブリック関数**//

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private: //**プライベート変数**//

	//開始時間、終了時間、頻度
	LARGE_INTEGER startTime, endTime, frequency;


};