#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

///<summary>
//������
///<summary>
void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

#pragma	region DirectInput�̏�����
	//DirectInput�̏�����
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion
}

///<summary>
//�X�V
///<summary>
void Input::Update()
{
	//�L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();

	//�S�L�[�̓��͏�Ԃ��擾����
	
	for (int i = 0; i < 256; ++i)
	{
		preKeys[i] = keys[i];
	}

	keyboard->GetDeviceState(sizeof(keys), keys);

}
///<summary>
///�L�[��������Ă��邩
///<summary>
bool Input::PushKey(BYTE keyNumber) {

	//�w��L�[�������Ă����true
	if (keys[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}