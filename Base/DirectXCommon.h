#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXCommon
{
public: // 省略
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	DirectXCommon();
	~DirectXCommon();

	//初期化
	void Initialize();
	//コマンド関連の初期化
	void InitializeCommand();
	//スワップチェーンの初期化
	void InitializeSwapchain();
private:
	//DirectX12デバイス
	ComPtr<ID3D12Device> device = nullptr;
	//DXGIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
};

DirectXCommon::DirectXCommon()
{
}

DirectXCommon::~DirectXCommon()
{
}