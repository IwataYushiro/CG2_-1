#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>

//�X�v���C�g��ՃN���X

class SpriteCommon
{
public: // �����o�֐�
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://�����o�֐�

	SpriteCommon();
	~SpriteCommon();

	//������
	void Initialize(DirectXCommon* dxCommon);

private://�����o�ϐ�

	DirectXCommon* dxCommon_ = nullptr;

	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	//�p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;

public://�A�N�Z�b�T

	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
