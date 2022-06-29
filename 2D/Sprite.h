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
class Sprite
{
public: // メンバ関数
	Sprite();
	~Sprite();
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
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT2 uv;		//uv座標
	};

	//頂点データ
	Vertex vertices_[4] = {
		// x	 y	  z		 u	  v
		{{-0.5f,-0.7f,0.0f},{0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-0.5f,+0.7f,0.0f},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{+0.5f,-0.7f,0.0f},{1.0f,1.0f}},//右下					
		{{+0.5f,+0.7f,0.0f},{1.0f,0.0f}},//右上					
	};
	
	//インデックスデータ
	unsigned short indices[6] =
	{
		0,1,2,//三角形1つ目
		1,2,3,//三角形2つ目
	};

	ID3D12Resource* constBuffMaterial = nullptr;

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW idView{};

	//設定をもとにSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;

	//パイプラインステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
};
