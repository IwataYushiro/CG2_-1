#pragma once
#include <Windows.h>

//����
class Input
{
public:
	Input();
	~Input();
	
	//������
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	
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