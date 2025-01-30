#include"base/memoryReakChecker.h"
#include"MainSystem/MainSystem.h"
#include"SingleGameObjects/Model.h"
#include<dxgidebug.h>

using namespace Engine;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	RegisterLeakCheck();

	MainSystem* Engine = MainSystem::GetInstance();

	Engine->Run();

	return 0;
}