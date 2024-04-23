#include"SceneManager/SceneManager.h"

#include<imgui.h>


#pragma region シーンのh
#include"Scenes/DebugScene.h"
#include"CG/CGScene.h"
#include"AL/ALScene/ALTitleScene.h"
#include"AL/ALScene/ALGameScene.h"
#pragma endregion



SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{

	//シーンの数取得
	sceneArr_.resize((size_t)SCENE::_SceneCount);

	//各シーンの情報設定
	sceneArr_[DEBUG] = std::make_unique<DebugScene>();
	sceneArr_[ALTITLE] = std::make_unique<ALTitleScene>();
	sceneArr_[ALGAME] = std::make_unique<ALGameScene>();
	sceneArr_[CG] = std::make_unique<CGScnene>();

	sceneName_.clear();
	sceneName_.push_back("DEBUG");
	sceneName_.push_back("ALTITLE");
	sceneName_.push_back("ALGAME");
	sceneName_.push_back("CG");

	//初期シーン設定
	IScene::SetSceneNo(CG);

}

void SceneManager::Update()
{
	
	//シーンチェック
	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = IScene::GetSceneNo();
	
	//デバッグ表示
	DebugWindow();

	//シーン変更チェック
	if (prevSceneNo_ != currentSceneNo_)
	{
		//変更していたら		
		//初期化処理
		sceneArr_[currentSceneNo_]->Initialize();
	}

	
	//シーン更新処理
	sceneArr_[currentSceneNo_]->Update();


}

void SceneManager::Draw()
{
	//描画処理
	sceneArr_[currentSceneNo_]->Draw();

}

void SceneManager::DebugWindow()
{
#ifdef _DEBUG

	int num = IScene::GetSceneNo();

	ImGui::Begin("SceneManager");
	ImGui::Text("SceneNo.%d", currentSceneNo_);
	ImGui::Text("%s", sceneName_[currentSceneNo_].c_str());
	ImGui::SliderInt("sceneNo", &num,0,_SceneCount-1 );
	ImGui::End();

	IScene::SetSceneNo(num);
#endif // _DEBUG

}

