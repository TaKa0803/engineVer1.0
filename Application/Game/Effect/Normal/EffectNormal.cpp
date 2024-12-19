#include "EffectNormal.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"
#include"GlobalVariable/Group/GlobalVariableGroup.h"
#include<cassert>

EffectNormal::EffectNormal(const std::string& tag,const std::string& name)
{
	//インスタンシングモデルマネージャのインスタンス取得
	IMM_ = InstancingModelManager::GetInstance();
	//タグの検索
	if (!IMM_->SerchTag(tag)) {
		//見つからない
		assert(false);
	}
	//タグ設定
	tag_ = tag;

	//デバッグ用にパラメータをツリーに追加
	std::unique_ptr<GlobalVariableGroup> tree = std::make_unique<GlobalVariableGroup>(name);
	tree->SetValue("posMax", &emitData_.spawnMaxWide);
	tree->SetValue("posMin", &emitData_.spawmMinWide);
	tree->SetValue("size", &emitData_.size);
	tree->SetValue("spawm", &emitData_.numSpawn);
	tree->SetValue("maxVeloDirec", &emitData_.veloMax);
	tree->SetValue("minVeloDirec", &emitData_.veloMin);
	tree->SetValue("acce", &emitData_.acce);
	tree->SetValue("spd", &emitData_.spd);
	tree->SetValue("liveTime", &emitData_.deadCount);
	tree->SetTreeData(IMM_->CreateAndGetTree(tag_, "model"));
}

void EffectNormal::Initialize()
{
	//残っているエフェクトデータ削除
	datas_.clear();
}

void EffectNormal::Update()
{
	//現存するエフェクトすべての更新処理
	for (auto& data : datas_) {
		//カウントチェック
		if (data.currentCount >= data.count) {
			//死亡フラグON
			data.isDead=true;
		}
		else {
			//カウント進行
			data.currentCount += (float)DeltaTimer::deltaTime_;
			
			//速度に加速度加算
			data.velo += data.acce * (float)DeltaTimer::deltaTime_;
			//速度分有働
			data.world.translate_ += data.velo * (float)DeltaTimer::deltaTime_;
			//行列更新
			data.world.UpdateMatrix();

			//高さが0以下の時0でとどまるように
			if (data.world.translate_.y < 0) {
				data.world.translate_.y = 0;
			}
		}
	}

	//死亡フラグがONのものを削除
	datas_.remove_if([](EffectData data) {
		if (data.isDead) {
			return true;
		}
		else { return false; }

		});

}

void EffectNormal::Draw()
{
	//すべてのモデル描画
	for (auto& data : datas_) {
		//データを送信
		IMM_->SetData(tag_, data.world,data.color);
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

		//データ追加
		datas_.push_back(data);
	}

}


