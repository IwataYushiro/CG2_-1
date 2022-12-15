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

	//�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//�p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState;
//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob = nullptr;

public://�A�N�Z�b�T

	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
