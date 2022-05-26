#pragma once
#include <Windows.h>
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
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private://メンバ変数
	HRESULT result;
	ID3D12Device* device = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	
	XMFLOAT3 vertices[3][10] = {
		{
		{-0.5f,-0.5f,0.0f},//左下	Xが-で左　Yが-で下
		{-0.5f,+0.5f,0.0f},//左上	Xが-で左　Yが+で上
		{+0.5f,-0.5f,0.0f},
		}
		//右下	Xが+で右　Yが-で下
	};
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB;

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};		//ヒープ設定
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};

	//頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;

	//定数バッファの設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定

	ID3D12Resource* constBuffMaterial = nullptr;
	
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC* inputLayout = nullptr;

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	ID3DBlob* vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;	//エラーオブジェクト

};

