#pragma once
#include <Windows.h>

class WinApp
{
public:
	WinApp();
	~WinApp();
	//�E�B���h�D�v���V�[�W��
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//������
	void Initialize();

	//�X�V
	void Update();
private:
	

};

WinApp::WinApp()
{
}

WinApp::~WinApp()
{
}