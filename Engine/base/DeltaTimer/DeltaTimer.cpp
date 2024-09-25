#include "DeltaTimer.h"

double DeltaTimer::deltaTime_ = 0.0f;

DeltaTimer* DeltaTimer::GetInstance()
{
	static DeltaTimer instance;
	return &instance;
}

void DeltaTimer::Initialize()
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startTime);
}

void DeltaTimer::Update()
{
	QueryPerformanceCounter(&endTime);
	// 経過時間を計算
	deltaTime_ = static_cast<double>(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
	// 現在の時間を次回の開始時間として設定
	startTime = endTime;
}


