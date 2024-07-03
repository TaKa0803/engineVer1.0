#pragma once
#include"DirectXFunc/DirectXFunc.h"

class IPipelineStateObject {

protected:

	DirectXFunc* DXF_ = nullptr;

public:

	virtual void Initialize() = 0;
	virtual void PreDraw() = 0;;

	virtual void Debug() = 0;
};