#pragma once
#include <Windows.h>

class WinApp
{
public://�萔

public://�����o�֐�
	WinApp();
	~WinApp();

	//�E�B���h�D�v���V�[�W��
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//������
	void Initialize();

	//�X�V
	void Update();

private://�����o�ϐ�
	//�E�B���h�D�n���h��
	HWND hwnd = nullptr;

public://�A�N�Z�b�T�u����
	HWND GetHwnd() const { return hwnd; }
};

WinApp::WinApp()
{
}

WinApp::~WinApp()
{
}