#include "DebugScene.h"
#include"TextureManager/TextureManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include"LightManager/LightManager.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"

#include"GvariGroup/GvariGroup.h"

DebugScnene::DebugScnene()
{
	//入力のインスタンス取得
	input_ = Input::GetInstance();

	//カメラのインスタンス取得
	camera_ = Camera::GetInstance();

	//オブジェクト生成
	object = std::make_unique<GameObject>();

	//山オブジェクト生成
	terrain = new GameObject();

	//スカイボックス生成
	skybox_ = new SkyBoxModel("resources/Texture/CG/rostock_laage_airport_4k.dds");

	//MapLoader::GetInstance()->LoadLevelEditor("untitled",".json");
	//MapLoader::GetInstance()->CreateModel(0);

	//パーティクルマネージャ生成
	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);
}

DebugScnene::~DebugScnene() { 
	//解放処理
	delete terrain;
	delete skybox_; 
}

void DebugScnene::Initialize()
{

	//動きチェック用で複数のモデルタグの初期化パターンをコメントとして残す
	//object->Initialize("AnimeCube");
	//object->Initialize("sphere");
	//object->Initialize("walk");
	object->Initialize("human");
	//object->Initialize("Boss");

	//山初期化
	terrain->Initialize("terrain");

	//カメラ初期化
	camera_->Initialize();
	//タゲ設定
	camera_->SetTarget(&object->world_);

	//スカイボックス初期化
	skybox_->Initialize();
	//サイズ設定
	skybox_->world_.scale_ = { 500,500,500 };

	//ポイイトライト初期化
	pointLight_ = PointLight();

	//パーティクルが関数呼び出しのみエフェクト発生するよう処理
	particleManager_->SetOnlyImpact(true);

	//デバッグ用にパラメータ設定
	std::unique_ptr<GlobalVariableGroup>gvg = std::make_unique<GlobalVariableGroup>("DebugScene");
	gvg->SetTreeData(object->model_->SetDebugParam());
	gvg->SetValue("オブジェクト回転",&object->world_.rotate_);
}

void DebugScnene::Update()
{
	//デバッグ処理
	Debug();

	//パーティクルデバッグ
	particleManager_->Debug("particle");

	//移動入力取得
	Vector3 move = input_->GetAllArrowKey();
	move.y = input_->GetWASD().z;
	//ポイントライトの位置移動
	pointLight_.position += move;

	//各オブジェクト更新
	object->Update();
	terrain->Update();
	skybox_->Update();
	camera_->Update();

	//ライトを設定
	LightManager::GetInstance()->SetPLight(pointLight_);
	LightManager::GetInstance()->SetDLight(dLight_);
	
	//パーティクルを更新
	particleManager_->Update();
	
	//スペースキーでエフェクト発生
	if (input_->PushKey(DIK_SPACE)) {
		particleManager_->SpawnE({0,0,0});
	} 
}



void DebugScnene::Draw()
{
	
	//skybox_->Draw();	

	//terrain->Draw();
	//モデル描画
	object->Draw();

	//MapLoader::GetInstance()->DrawLevelData();
	//現在のすべてのインスタンシングモデル描画
	InstancingModelManager::GetInstance()->DrawAllModel();

	//パーティクル描画
	particleManager_->Draw();
	
	//現在のすべてのインスタンシングモデル描画
	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kBloom, true);
}

void DebugScnene::Debug()
{
	//PMのデバッグ処理
	PostEffectManager::GetInstance()->Debug();
	//PostEffectManager::GetInstance()->Debug(PostEffectManager::kVinetting);

	//camera_->DrawDebugWindow("camera");

	//各ライトのデバッグ処理
	ImGui::Begin("Light");
	ImGui::Text("PointLight");
	ImGui::DragFloat("p light intencity", &pointLight_.intensity, 0.01f);
	ImGui::DragFloat3("p light pos", &pointLight_.position.x, 0.1f);
	ImGui::ColorEdit4("light color", &pointLight_.color.x);
	ImGui::DragFloat("p light radius", &pointLight_.radius, 0.1f);
	ImGui::DragFloat("p light decay", &pointLight_.decay, 0.1f);

	ImGui::Text("DirectionalLight");
	ImGui::DragFloat("d light intencity", &dLight_.intensity, 0.01f);
	ImGui::ColorEdit4("light color", &dLight_.color.x);
	ImGui::DragFloat3("d direction", &dLight_.direction.x, 0.1f);

	ImGui::End();

	dLight_.direction.SetNormalize();
}
