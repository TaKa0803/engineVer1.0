#pragma once
#include"Sprite/Sprite.h"

#include"2d/SpritePSO/SpritePSO.h"

class SpriteManager {

public://シングルトンパターン
	static SpriteManager* GetInstance();

private:
	SpriteManager() = default;
	~SpriteManager() = default;
	SpriteManager(const SpriteManager& o) = delete;
	const SpriteManager& operator=(const SpriteManager& o) = delete;

public:
	//初期化
	void Initialize();

	void Finalize();

	//描画前処理
	static void PreDraw( BlendMode blendMode = BlendMode::kNormal);

private:
	//

	SpritePSO* grarphics_;

};