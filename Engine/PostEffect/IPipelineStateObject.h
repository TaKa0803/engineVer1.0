#pragma once
#include"DirectXFunc/DirectXFunc.h"


class IPipelineStateObject {

protected://**共通変数**//

	//DXFのポインタ
	Engine::DirectXFunc* DXF_ = nullptr;

public://**パブリック関数
	
	/// <summary>
	/// 描画前処理
	/// </summary>
	virtual void PreDraw() = 0;;

	/// <summary>
	/// デバッグ処理
	/// </summary>
	virtual void Debug() = 0;

	/// <summary>
	/// 解放処理
	/// </summary>
	virtual void Release() = 0;
};