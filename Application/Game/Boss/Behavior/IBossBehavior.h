#pragma once
#include"GvariGroup/GvariGroup.h"

class Boss;

class IBossBehavior {

public:
	virtual ~IBossBehavior() = default;         // 純粋仮想関数

	//初期化
	virtual void Initialize()=0;

	//更新
	virtual void Update()=0;

	//ツリー取得
	GVariTree& GetTree() { return tree_; };

public:
	GVariTree tree_ = GVariTree("");
};