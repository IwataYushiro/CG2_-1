#pragma once
#include "SpriteCommon.h"
#include <DirectXMath.h>

//1��1���̃X�v���C�g�N���X

class Sprite
{
public: // �����o�֐�
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�����o�֐�
	Sprite();
	~Sprite();

	//������
	void Initialize(SpriteCommon* spCommon);

private://�����o�ϐ�

	SpriteCommon* spCommon_ = nullptr;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertbuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	
};
