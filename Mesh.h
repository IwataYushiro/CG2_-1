#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <DirectXMath.h>
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
	Mesh();
	~Mesh();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HRESULT result, ID3D12Device* device);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList);

private://メンバ変数

	XMFLOAT3 vertices_[3] = {
		{-0.5f,-0.5f,0.0f},//左下	Xが-で左　Yが-で下
		{-0.5f,+0.5f,0.0f},//左上	Xが-で左　Yが+で上
		{+0.5f,-0.5f,0.0f},//右下	Xが+で右　Yが-で下
	};

	ID3D12Resource* constBuffMaterial = nullptr;

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	
	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;

	//パイプラインステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
};

