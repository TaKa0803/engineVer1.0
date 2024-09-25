#pragma once
#include<Windows.h>



class DeltaTimer {

public://シングルトンパターン
	static DeltaTimer* GetInstance();

private://シングルトンパターン

	DeltaTimer() = default;
	~DeltaTimer() = default;
	DeltaTimer(const DeltaTimer& o) = delete;
	const DeltaTimer& operator=(const DeltaTimer& o) = delete;

public:

	void Initialize();

	void Update();

	static double deltaTime_;

private:

	LARGE_INTEGER startTime, endTime, frequency;


};