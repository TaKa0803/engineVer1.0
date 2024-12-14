#include "RandomNum.h"

RandomNumber* RandomNumber::GetInstance() {
	//インスタンス取得
	static RandomNumber instance;
	return&instance;
}

void RandomNumber::RandomNumberProcessInitialize() {
	//乱数生成変数の初期化
	std::random_device seedGenerator;
	std::mt19937_64 randomEngine(seedGenerator());

	randomEngine_ = randomEngine;
}

float RandomNumber::Get(const float minNum, const float maxNum) {
	//値をセット
	std::uniform_real_distribution<float>distribution(minNum, maxNum);
	//乱数を取得
	RandomNumber* rand = RandomNumber::GetInstance();
	return distribution(rand->GetrandEngine());
}



Vector2 RandomNumber::Get(const Vector2 minNum, const Vector2 maxNum) {
	//値をセット
	std::uniform_real_distribution<float>distributionx(minNum.x, maxNum.x);
	std::uniform_real_distribution<float>distributiony(minNum.y, maxNum.y);

	//乱数を取得
	RandomNumber* rand = RandomNumber::GetInstance();
	Vector2 ans = {
		distributionx(rand->GetrandEngine()),
		distributiony(rand->GetrandEngine())
	};

	return ans;
}

Vector3 RandomNumber::Get(const Vector3& minNum, const Vector3& maxNum) {
	//値をセット
	RandomNumber* rand = RandomNumber::GetInstance();
	return {
		rand->GetRandomNum(minNum.x,maxNum.x),
		rand->GetRandomNum(minNum.y,maxNum.y),
		rand->GetRandomNum(minNum.z,maxNum.z),
	};
}

Vector4 RandomNumber::Get(const Vector4& minNum, const Vector4& maxNum) {
	RandomNumber* rand = RandomNumber::GetInstance();
	//乱数を取得して返却
	return {
		rand->GetRandomNum(minNum.x,maxNum.x),
		rand->GetRandomNum(minNum.y,maxNum.y),
		rand->GetRandomNum(minNum.z,maxNum.z),
		rand->GetRandomNum(minNum.w,maxNum.w)
	};
}

float RandomNumber::GetRandomNum(const float min, const float max) {
	//最小、最大の関係を見直してから乱数を求める
	float newmin;
	float newmax;

	if (min > max) {
		newmin = max;
		newmax = min;
	}
	else {
		newmin = min;
		newmax = max;
	}

	std::uniform_real_distribution<float>distribution(newmin, newmax);
	return distribution(randomEngine_);
}


