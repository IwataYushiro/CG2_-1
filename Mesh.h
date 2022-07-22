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
#include <random>

using namespace DirectX;

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)

};
//座標
struct ConstBufferDataTransform
{
	XMMATRIX mat; // 3D変換行列

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
	
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;
	float alpha = 1.0f;

	//3Dオブジェクト型
	struct Object3d
	{
		//定数バッファ(行列用)
		ComPtr<ID3D12Resource> constBuffTransform = nullptr;
		//構造体を変数化
		ConstBufferDataTransform* cbdt = nullptr;
		//定数バッファマップ(行列用)
		ConstBufferDataTransform* constMapTransform = nullptr;
		//アフィン変換情報
		XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
		XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
		XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		//ワールド変換行列
		XMMATRIX matWorld = XMMatrixIdentity();
		//親オブジェクトへのポインタ
		Object3d* parent = nullptr;
	};
	Object3d object3d_;
public:
	Mesh();
	~Mesh();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HRESULT result, ID3D12Device* device);

	//定数バッファ作成
	void CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device);
	void CreateConstBufferObject3d(Object3d* object, ID3D12Device* device);
	//オブジェクトの初期化
	void SetObject3d();
	void InitViewProjection();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update(BYTE* keys, BYTE* preKeys, ID3D12Device* device);
	//マテリアルの更新
	void UpdateMaterial3d(Material3d* material);

	//オブジェクトの更新
	void UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matprojection);
	void ControlObject3d(Object3d* object, BYTE* keys);
	//ビュー操作
	void ControlView(BYTE* keys);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList);

	void DrawObject3d(Material3d* material, Object3d* object,
		ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vdView, D3D12_INDEX_BUFFER_VIEW& idView, UINT numIndices);

private://メンバ変数
	//頂点数
	static const size_t VerticesCount_ = 4;
	//インデックス数
	static const size_t indicesCount_ = 3;

	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT3 normal;	//法線ベクトル
		XMFLOAT2 uv;		//uv座標
	};

	Vertex vertices_[VerticesCount_] = {
		//前(Z固定)
		// x	 y	  z		  法線  u	  v
		{{-50.0f ,-50.0f ,50.0f},{}, {0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-50.0f,  50.0f ,50.0f},{}, {0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{ 50.0f ,-50.0f ,50.0f},{}, {1.0f,1.0f}},//右下	
		{{ 50.0f , 50.0f ,50.0f},{},{1.0f,0.0f}},//右上	
		
	};
	ComPtr<ID3D12Resource> vertBuff = nullptr;

	//インデックスデータ
	unsigned short indices[indicesCount_] =
	{
		//前
		0,1,2,//三角形1つ目
		
	};
	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff = nullptr;

	//射影変換行列
	XMMATRIX matprojection;

	//ビュー変換行列
	XMMATRIX matview;
	//視点
	XMFLOAT3 eye;
	//注視点
	XMFLOAT3 target;
	//上方向ベクトル
	XMFLOAT3 up;
	float angle = 0.0f;//カメラの回転角
	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};
	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW idView{};

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
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

