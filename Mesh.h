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


	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	ID3DBlob* vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;	//エラーオブジェクト

};

