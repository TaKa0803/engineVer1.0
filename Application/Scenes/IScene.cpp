#include"IScene/IScene.h"

int IScene::sceneNo = 0;

bool IScene::leaveGame = false;

IScene::~IScene() {}

int IScene::GetSceneNo() { return sceneNo; }

void IScene::SetSceneNo(int num) { sceneNo = num; }
