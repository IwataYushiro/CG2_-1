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
	//�R�}���h�֘A�̏�����
	void InitializeCommand();
	//�X���b�v�`�F�[���̏�����
	void InitializeSwapchain();
private:
	//DirectX12�f�o�C�X
	ComPtr<ID3D12Device> device = nullptr;
	//DXGI�t�@�N�g���[
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
};

DirectXCommon::DirectXCommon()
{
}

DirectXCommon::~DirectXCommon()
{
}