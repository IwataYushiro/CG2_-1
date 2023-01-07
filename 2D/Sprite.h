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

	//������
	void Initialize(SpriteCommon* spCommon);

	//�`��
	void Draw();

private://�����o�ϐ�
	//���_��
	static const size_t verticesCount = 3;

	//���_�f�[�^
	XMFLOAT3 vertices[verticesCount] = {
		{-0.5f,-0.5f,0.0f},
		{-0.5f,+0.5f,0.0f},
		{+0.5f,-0.5f,0.0f},
	};

	SpriteCommon* spCommon_ = nullptr;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	
};
