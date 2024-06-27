#pragma once

class PostEffectManager {
public:

	static PostEffectManager* GetInstance();

private:
	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(const PostEffectManager& o) = delete;
	const PostEffectManager& operator=(const PostEffectManager& o) = delete;

public:

	void Initialize();

	void PostEffectDraw();


};