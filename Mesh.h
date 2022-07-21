#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dinput.h>
#include <math.h>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl.h>

using namespace DirectX;

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)

};
class Mesh
{
public: // メンバ関数
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
private:
	struct Material3d
	{
		//定数バッファのGPUリソースのポインタ
		ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

		//構造体を変数化
		ConstBufferDataMaterial* cbdm = nullptr;

		//マッピング用のポインタ
		ConstBufferDataMaterial* constMapMaterial = nullptr;

	};
	//3Dマテリアル
	Material3d material3d_;
public:
	Mesh();
	~Mesh();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HRESULT result, ID3D12Device* device);

	void CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device);
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList);

private://メンバ変数
	//頂点数
	static const size_t VerticesCount_ = 3;

	XMFLOAT3 vertices_[VerticesCount_] = {
		{-0.5f,-0.5f,0.0f},//左下	Xが-で左　Yが-で下
		{-0.5f,+0.5f,0.0f},//左上	Xが-で左　Yが+で上
		{+0.5f,-0.5f,0.0f},//右下	Xが+で右　Yが-で下
	};

	ComPtr<ID3D12Resource> vertBuff = nullptr;

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	//頂点シェーダ等
	ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
};

