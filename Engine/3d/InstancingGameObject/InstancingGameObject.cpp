#include "InstancingGameObject/InstancingGameObject.h"
#include<cassert>
#include<imgui.h>

GlobalVariableTree& InstancingGameObject::GetDebugTree(const std::string& name)
{
	//名前設定
	tree_.name_ = name;
	//ワールドのツリー追加
	tree_.SetTreeData(world_.GetDebugMonitorTree());

	return tree_;
}

InstancingGameObject::~InstancingGameObject() {
}

void InstancingGameObject::Initialize(const std::string& tag) {
	//インスタンシングモデルマネージャのインスタンス取得
	IMM_ = InstancingModelManager::GetInstance();

	//タグが存在しているかチェック
	if (IMM_->SerchTag(tag)) {
		tag_ = tag;
	}
	else {
		assert(false);
	}
}

void InstancingGameObject::Update() {
	//行列更新
	world_.UpdateMatrix();

}

void InstancingGameObject::Draw()
{
	//タグに対応したモデルにワールド追加
	IMM_->SetData(tag_, world_, color_);
}

void InstancingGameObject::Debug(const char* name)
{
	name;
#ifdef _DEBUG
	//デバッグUI表示
	if (ImGui::BeginMenu(name)) {

		IMM_->Debug(tag_, name);

		//world_.DrawDebug(name);

		ImGui::EndMenu();
	}
#endif // _DEBUG
}



