#pragma once
#define DIRECTINPUT_VERSION		0x0800		//DirectInput�̃o�[�W�����w��
#include <cassert>
#include <dinput.h>
#include <wrl.h>

using namespace Microsoft::WRL;

//����
class Input
{
public:
	Input();
	~Input();
	
	//������
	void Initialize();
	
	//�X�V
	void Update();
private:

};

Input::Input()
{
}

Input::~Input()
{
}