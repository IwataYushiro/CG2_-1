#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>

#include "WinApp.h"

class DirectXCommon
{
public: // �ȗ�
	//�G�C���A�X�e���v���[�g
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	DirectXCommon();
	~DirectXCommon();

	//������
	void Initialize(WinApp* winApp);
	//�f�o�C�X�̏�����
	void InitializeDevice();
	//�R�}���h�֘A�̏�����
	void InitializeCommand();
	//�X���b�v�`�F�[���̏�����
	void InitializeSwapchain();
	//�����_�[�^�[�Q�b�g�r���[�̏�����
	void InitializeRenderTargetView();
	//�[�x�o�b�t�@�̏�����
	void InitializeDepthBuffer();
	//�t�F���X������
	void InitializeFence();

	//�`��O����
	void PreDraw();
	//�`��㏈��
	void PostDraw();
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
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	
	//�����_�[�^�[�Q�b�g�r���[
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	//�[�x�r���[�p
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	// �o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers;

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};

DirectXCommon::DirectXCommon()
{
}

DirectXCommon::~DirectXCommon()
{
}