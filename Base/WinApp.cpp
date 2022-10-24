#include "WinApp.h"

//�E�B���h�D�v���V�[�W��
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg)
	{
		//�E�B���h�D���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��ăA�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//������
void WinApp::Initialize() {
	//�E�B���h�D�T�C�Y
	const int window_width = 1280; //����
	const int window_height = 720; //�c��

	//�E�B���h�D�N���X�̐ݒ�
	WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;      //�E�B���h�D�v���V�[�W����ݒ� 
	w.lpszClassName = L"DX12Sample";           //�E�B���h�D�N���X��
	w.hInstance = GetModuleHandle(nullptr);   //�E�B���h�D�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);  //�J�[�\���w��

	//�E�B���h�D�N���X��OS�ɓo�^
	RegisterClassEx(&w);
	//�E�B���h�E�T�C�Y {X���W,Y���W,����,�c��}
	RECT wrc = { 0,0,window_width,window_height };
	//�����ŃT�C�Y��␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�D�쐬
	HWND hwnd = CreateWindow(w.lpszClassName,   //�N���X��
		L"DirectXGame",							//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,					//�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,							//�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,							//�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,					//�E�B���h�E����
		wrc.bottom - wrc.top,					//�E�B���h�E�c��
		nullptr,								//�e�E�B���h�E�n���h��
		nullptr,								//���j���[�n���h��
		w.hInstance,							//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);								//�I�v�V����

	//�E�B���h�D��\����Ԃɂ���
	ShowWindow(hwnd, SW_SHOW);

	MSG msg{}; //���b�Z�[�W
}

//�X�V
void WinApp::Update() {

}
