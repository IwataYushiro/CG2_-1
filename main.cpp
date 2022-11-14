#include "WinApp.h"
#include "DirectXCommon.h"
#include "Mesh.h"
#include "Input.h"

const float PI = 3.141592f;					//const float�ł���

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

// Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#pragma region �|�C���^�u����
	WinApp* winApp = new WinApp();
	
	DirectXCommon* dxCommon = new DirectXCommon();

	Input* input = new Input();
	Mesh* mesh = new Mesh();


#pragma endregion

#pragma region Windows������
winApp->Initialize();
#pragma endregion
	// DirectX�����������@��������

	//DirectX������
	dxCommon->Initialize(winApp);
	// DirectX�����������@�����܂�

	// �`�揉���������@��������
#pragma region �`�揉��������
	
	input->Initialize(winApp);
	mesh->Initialize(winApp, dxCommon);

#pragma endregion
	// �`�揉���������@�����܂�

	//�Q�[�����[�v
	while (true)
	{
		//Windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage())
		{
			//�Q�[�����[�v�𔲂���
			break;
		}

		mesh->Update(dxCommon->GetDevice());
		// DirectX���t���[�������@��������
		//�`��O����
		dxCommon->PreDraw();

		mesh->Draw(dxCommon->GetCommandList());

		//�`��㏈��
		dxCommon->PostDraw();
		// 4.�`��R�}���h�����܂�

		// DirectX���t���[�������@�����܂�

	}
	winApp->Finalize();

	//���
	delete winApp;
	delete dxCommon;
	delete mesh;
	delete input;
	//�R���\�[���ւ̕����o��
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}