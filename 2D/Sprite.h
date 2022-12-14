#pragma once
#include "SpriteCommon.h"

//1枚1枚のスプライトクラス

class Sprite
{
public://メンバ関数
	Sprite();
	~Sprite();

	//初期化
	void Initialize(SpriteCommon* spCommon);

private://メンバ変数

	SpriteCommon* spCommon_ = nullptr;


	
};
