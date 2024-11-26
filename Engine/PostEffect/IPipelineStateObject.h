#pragma once
#include"DirectXFunc/DirectXFunc.h"


class IPipelineStateObject {

protected:

	DirectXFunc* DXF_ = nullptr;

public:
	
	
	virtual void PreDraw() = 0;;

	virtual void Debug() = 0;

	virtual void Release() = 0;
};