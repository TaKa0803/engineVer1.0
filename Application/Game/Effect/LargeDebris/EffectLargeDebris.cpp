#include "EffectLargeDebris.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"
#include"TextureManager/TextureManager.h"

EffectLargeDebris::EffectLargeDebris()
{
	InstancingGameObject::Initialize("Debris");

	IMM_->SetTexture(tag_, TextureManager::LoadTex("resources/Texture/AL/whiteGrid.png").texNum);
	IMM_->SetEnableShader(tag_, false);
	IMM_->SetEnableTexture(tag_, false);


	tree_.SetValue("出現最大範囲", &spawnWide_);
	tree_.SetValue("最大初速度", &spd_);
	tree_.SetValue("重力", &gravity_);
	tree_.SetValue("同時出現数", &spawnSameTimeNum_);
	tree_.SetValue("生存時間", &liveSec_);
	tree_.SetValue("エフェクトの色1", &color1_);
	tree_.SetValue("エフェクトの色2", &color2_);
	tree_.SetValue("エフェクトの色3", &color3_);

}

void EffectLargeDebris::Init()
{
	effects_.clear();
}

void EffectLargeDebris::Update()
{
	//登録されているパラメータをまとめて行進
	for (auto& param : effects_) {
		//速度を設定
		param.velo += param.acce * (float)DeltaTimer::deltaTime_;

		//移動量を渡す
		param.world.translate_ += param.velo * (float)DeltaTimer::deltaTime_;

		//0以下の場合いにする
		if (param.world.translate_.y <= 0) {
			param.world.translate_.y = 0;

			//地面についてからカウント開始
			//生存時間立ったら処理
			if ((param.liveCount -= (float)DeltaTimer::deltaTime_) <= 0.0f) {
				param.isDead = true;
			}
		}


	}

	//削除処理
	effects_.remove_if([](Parameter param) {

		if (param.isDead) {
			return true;
		}
		else { return false; }

		});

}

void EffectLargeDebris::Draw()
{
	for (auto& data : effects_) {
		data.world.UpdateMatrix();

		IMM_->SetData(tag_, data.world,0,data.color);
	}
}

void EffectLargeDebris::Spawn(const Vector3& pos)
{
	//新しく追加
	int num = (int)RandomNumber::Get(spawnSameTimeNum_.x, spawnSameTimeNum_.y);

	for (int i = 0; i < num;i++) {
		//初期位置を生成
		Parameter newParam{};

		//二つのランダムな値を取得
		float randX = RandomNumber::Get(-1.0f, 1.0f);
		float randZ = RandomNumber::Get(-1.0f, 1.0f);
		//ランダムな方向を作成
		Vector3 p = { randX,0,randZ };
		p.SetNormalize();

		float t = RandomNumber::Get(0.0f, 1.0f);

		float wide = RandomNumber::Get(spawnWide_.x, spawnWide_.y);

		p *= Lerp(0, wide,t);

		newParam.world.translate_ = pos+p;
		//真上に飛ぶ
		newParam.velo.y = 1.0f;
		//初速度追加
		newParam.velo *= RandomNumber::Get(spd_.x,spd_.y);

		//重力追加
		newParam.acce.y = -gravity_;

		//生存時間の追加
		newParam.liveCount = liveSec_;

		float get =RandomNumber::Get(0, 3);

		if (get < 1.0f) {
			newParam.color = color1_;
		}
		else if (get < 2.0f) {
			newParam.color = color2_;
		}
		else if (get < 3.0f) {
			newParam.color = color3_;
		}

		//値追加
		effects_.push_back(newParam);
	}
}
