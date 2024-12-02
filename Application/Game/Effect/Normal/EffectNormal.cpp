#include "EffectNormal.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"
#include<cassert>

EffectNormal::EffectNormal(const std::string& tag,const std::string& name)
{
	IMM_ = InstancingModelManager::GetInstance();
	//タグの検索
	if (!IMM_->SerchTag(tag)) {
		assert(false);
	}
	tag_ = tag;


	std::unique_ptr<GVariGroup> tree = std::make_unique<GVariGroup>(name);
	tree->SetValue("posMax", &emitData_.spawnMaxWide);
	tree->SetValue("posMin", &emitData_.spawmMinWide);
	tree->SetValue("size", &emitData_.size);
	tree->SetValue("spawm", &emitData_.numSpawn);
	tree->SetValue("最大ベクトル方向", &emitData_.veloMax);
	tree->SetValue("最小ベクトル方向", &emitData_.veloMin);
	tree->SetValue("加速度", &emitData_.acce);
	tree->SetValue("速度", &emitData_.spd);
	tree->SetValue("生存時間", &emitData_.deadCount);
	tree->SetTreeData(IMM_->CreateAndGetTree(tag_, "モデル"));
}

void EffectNormal::Initialize()
{
	datas_.clear();
}

void EffectNormal::Update()
{
	for (auto& data : datas_) {
		//カウントチェック
		if (data.currentCount >= data.count) {
			data.isDead=true;
		}
		else {
			//カウント進行
			data.currentCount += (float)DeltaTimer::deltaTime_;
			//移動処理
			data.velo += data.acce * (float)DeltaTimer::deltaTime_;
			data.world.translate_ += data.velo * (float)DeltaTimer::deltaTime_;
			data.world.UpdateMatrix();

			if (data.world.translate_.y < 0) {
				data.world.translate_.y = 0;
			}
		}
	}

	datas_.remove_if([](EffectData data) {
		if (data.isDead) {
			return true;
		}
		else { return false; }

		});

}

void EffectNormal::Draw()
{
	for (auto& data : datas_) {
		IMM_->SetData(tag_, data.world,0,data.color);
	}
}

void EffectNormal::SpawnEffect(const Vector3&pos)
{
	//生成数設定
	int num = (int)RandomNumber::Get(emitData_.numSpawn.x, emitData_.numSpawn.y);

	//数分生成
	for (int i = 0; i < num; i++) {
		//エフェクト出現処理
		EffectData data;
		//出現範囲
		data.world.translate_ = pos + RandomNumber::Get(emitData_.spawmMinWide, emitData_.spawnMaxWide);
		//サイズ
		data.world.SetScale(RandomNumber::Get(emitData_.size.x, emitData_.size.y));
		//向きベクトルと初速度
		data.velo = RandomNumber::Get(emitData_.veloMin, emitData_.veloMax).SetNormalize() * RandomNumber::Get(emitData_.spd.x, emitData_.spd.y);
		//加速度
		data.acce = emitData_.acce;
		//生存時間設定
		data.count = RandomNumber::Get(emitData_.deadCount.x, emitData_.deadCount.y);

		datas_.push_back(data);
	}

}


