#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>

//スプライト基盤クラス

class SpriteCommon
{
public://メンバ関数

	SpriteCommon();
	~SpriteCommon();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

private://メンバ変数

	DirectXCommon* dxCommon_ = nullptr;

public://アクセッサ

	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
