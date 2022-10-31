#pragma once
#include <Windows.h>

class WinApp
{
public://�萔
	//�E�B���h�D�T�C�Y
	static const int window_width = 1280; //����
	static const int window_height = 720; //�c��

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

	//�E�B���h�D�N���X�̐ݒ�
	WNDCLASSEX wndClassEx{};

	MSG msg{}; //���b�Z�[�W

public://�A�N�Z�b�T�u����
	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHinstance() const { return wndClassEx.hInstance; }

	MSG GetMsg() const { return msg; }
};

WinApp::WinApp()
{
}

WinApp::~WinApp()
{
}