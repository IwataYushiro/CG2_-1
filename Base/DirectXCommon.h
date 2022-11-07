#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXCommon
{
public: // �ȗ�
	//�G�C���A�X�e���v���[�g
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	DirectXCommon();
	~DirectXCommon();

	//������
	void Initialize();

private:
	//DirectX12�f�o�C�X
	ComPtr<ID3D12Device> device = nullptr;
	//DXGI�t�@�N�g���[
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
};

DirectXCommon::DirectXCommon()
{
}

DirectXCommon::~DirectXCommon()
{
}