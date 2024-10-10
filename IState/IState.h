#pragma once

class IState {

protected:
	//終了感知フラグ
	static bool isFinished_;

public:

	/// <summary>
	/// Stateの必須初期化処理
	/// </summary>
	void IStateInit();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init()=0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;



};