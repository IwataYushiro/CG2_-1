#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInput�̃o�[�W�����w��

#include <dinput.h>
#include <Windows.h>
#include <wrl.h>

//����
class Input
{
public:
	//�G�C���A�X�e���v���[�g��namespace�ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Input();
	~Input();
	
	///<summary>
	//������
	///<summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	
	///<summary>
	//�X�V
	///<summary>
	void Update();

	///<summary>
	///�L�[��������Ă��邩
	///<summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNumber);

private://�����o�ϐ�
	//�L�[�{�[�h
	ComPtr <IDirectInputDevice8> keyboard = nullptr;
	
	//�S�L�[�̏��
	BYTE preKeys[256] = {};
	BYTE keys[256] = {};

};

Input::Input()
{
}

Input::~Input()
{
}