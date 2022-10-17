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
	
	//������
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	
	//�X�V
	void Update();

private://�����o�ϐ�
	ComPtr <IDirectInputDevice8> keyboard = nullptr;

};

Input::Input()
{
}

Input::~Input()
{
}