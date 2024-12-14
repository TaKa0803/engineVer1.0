#pragma once
#include<random>
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"struct.h"

//乱数取得クラス
class RandomNumber {

public://シングルトンパターン
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static RandomNumber* GetInstance();
private://シングルトンパターン
	RandomNumber() = default;
	~RandomNumber() = default;
	RandomNumber(const RandomNumber& o) = delete;
	const RandomNumber& operator=(const RandomNumber& o) = delete;

public:	//**パブリック関数**//

	/// <summary>
	/// エンジン側での初期化
	/// </summary>
	void RandomNumberProcessInitialize();

	/// <summary>
	/// ランダムな数字を取得
	/// </summary>
	/// <param name="minNum">最小値</param>
	/// <param name="maxNum">最大値</param>
	static float Get(const float minNum, const float maxNum);

	/// <summary>
	/// ランダムな数字を取得
	/// </summary>
	/// <param name="minNum">最小値</param>
	/// <param name="maxNum">最大値</param>
	/// <returns>ランダムな数字</returns>
	static Vector2 Get(const Vector2 minNum, const Vector2 maxNum);

	/// <summary>
	/// ランダムな数字を取得
	/// </summary>
	/// <param name="minNum">最小値</param>
	/// <param name="maxNum">最大値</param>
	/// <returns>ランダムな数字</returns>
	static Vector3 Get(const Vector3 &minNum, const Vector3& maxNum);

	/// <summary>
	/// ランダムな数字を取得
	/// </summary>
	/// <param name="minNum">最小値</param>
	/// <param name="maxNum">最大値</param>
	/// <returns>ランダムな数字</returns>
	static Vector4 Get(const Vector4& minNum, const Vector4& maxNum);


private: //**プライベート関数**//

	/// <summary>
	/// randomEngine取得
	/// </summary>
	/// <returns></returns>
	std::mt19937_64& GetrandEngine() { return randomEngine_; }

	/// <summary>
	/// 乱数取得
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	float GetRandomNum(const float min, const float max);

private://**プライベート変数**//

	//乱数エンジン
	std::mt19937_64 randomEngine_;
	//乱数獲得処理
	std::uniform_real_distribution<float>distribution(float x, float y);
	
};


