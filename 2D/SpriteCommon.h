#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>

//�X�v���C�g��ՃN���X

class SpriteCommon
{
public: 
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://�����o�֐�

	//������
	void Initialize(DirectXCommon* dxCommon);

	//�`��O����
	void PreDraw();

private://�����o�ϐ�

	DirectXCommon* dxCommon_ = nullptr;

	
	//�p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState;
//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

public://�A�N�Z�b�T

	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
