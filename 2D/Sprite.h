#pragma once
#include "SpriteCommon.h"

//1��1���̃X�v���C�g�N���X

class Sprite
{
public://�����o�֐�
	Sprite();
	~Sprite();

	//������
	void Initialize(SpriteCommon* spCommon);

private://�����o�ϐ�

	SpriteCommon* spCommon_ = nullptr;


	
};
