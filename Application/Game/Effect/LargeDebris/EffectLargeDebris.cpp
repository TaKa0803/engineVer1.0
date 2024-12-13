#include "EffectLargeDebris.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"
#include"TextureManager/TextureManager.h"

EffectLargeDebris::EffectLargeDebris()
{
	//インスタンシングオブジェクトのタグがあっているか確認
	InstancingGameObject::Initialize("Debris");

	//影を無効化
	IMM_->SetEnableShader(tag_, false);
	//画像を無効化
	IMM_->SetEnableTexture(tag_, false);

	//デバッグ用にパラメータをツリーに追加
	tree_.SetValue("出現最大範囲", &spawnWide_);
	tree_.SetValue("最大初速度", &spd_);
	tree_.SetValue("重力", &gravity_);
	tree_.SetValue("同時出現数", &spawnSameTimeNum_);
	tree_.SetValue("生存時間", &liveSec_);
	tree_.SetValue("エフェクトの色1", &color1_);
	tree_.SetValue("エフェクトの色2", &color2_);
	tree_.SetValue("エフェクトの色3", &color3_);

}

void EffectLargeDebris::Initialize()
{
	//残っているエフェクトをすべて削除する
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

		//0以下の場合の処理
		if (param.world.translate_.y <= 0) {
			param.world.translate_.y = 0;

			//地面についてからカウント開始
			//生存時間立ったら処理
			if ((param.liveCount -= (float)DeltaTimer::deltaTime_) <= 0.0f) {
				//死亡フラグ有効
				param.isDead = true;
			}
		}


	}

	//削除処理
	effects_.remove_if([](Parameter param) {
		//死亡フラグ有効時削除
		if (param.isDead) {
			return true;
		}//無効なので削除処理を行わない
		else { return false; }

		});

}

void EffectLargeDebris::Draw()
{
	//すべての座標を送信
	for (auto& data : effects_) {
		//行列更新
		data.world.UpdateMatrix();
		//データ転送
		IMM_->SetData(tag_, data.world,data.color);
	}
}

void EffectLargeDebris::Spawn(const Vector3& pos)
{
	//新しく追加する数を取得
	int num = (int)RandomNumber::Get(spawnSameTimeNum_.x, spawnSameTimeNum_.y);

	for (int i = 0; i < num;i++) {
		//初期位置を生成
		Parameter newParam{};

		//二つのランダムな値を取得
		float randX = RandomNumber::Get(-1.0f, 1.0f);
		float randZ = RandomNumber::Get(-1.0f, 1.0f);
		//ランダムな出現場所を作成
		Vector3 p = { randX,0,randZ };
		p.SetNormalize();

		//割合Tを取得
		float t = RandomNumber::Get(0.0f, 1.0f);

		//長さ分伸ばす
		p *= Lerp(spawnWide_.x, spawnWide_.y,t);
		
		//伸ばした位置に出現する
		newParam.world.translate_ = pos+p;
		
		//真上に飛ぶ
		newParam.velo.y = 1.0f;
		//初速度追加
		newParam.velo *= RandomNumber::Get(spd_.x,spd_.y);

		//重力追加
		newParam.acce.y = -gravity_;

		//生存時間の追加
		newParam.liveCount = liveSec_;

		//色の変更処理
		float get =RandomNumber::Get(0, 3);

		//各範囲の時にいろを設定
		if (get < 1.0f) {
			newParam.color = color1_;
		}
		else if (get < 2.0f) {
			newParam.color = color2_;
		}
		else if (get <= 3.0f) {
			newParam.color = color3_;
		}

		//値追加
		effects_.push_back(newParam);
	}
}
