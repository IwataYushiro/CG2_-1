#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>

//�X�v���C�g��ՃN���X

class SpriteCommon
{
public://�����o�֐�

	SpriteCommon();
	~SpriteCommon();

	//������
	void Initialize(DirectXCommon* dxCommon);

private://�����o�ϐ�

	DirectXCommon* dxCommon_ = nullptr;

public://�A�N�Z�b�T

	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
