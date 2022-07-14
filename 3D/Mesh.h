#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dinput.h>
#include <DirectXMath.h>
#include <wrl.h>

using namespace DirectX;

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)

};
struct ConstBufferDataTransform
{
	XMMATRIX mat; // 3D変換行列

};
struct Material3d
{
	//定数バッファのGPUリソースのポインタ
	ID3D12Resource* constBuffMaterial = nullptr;
	
	//構造体を変数化
	ConstBufferDataMaterial* cbdm = nullptr;
	
	//マッピング用のポインタ
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	
};
//3Dオブジェクト型
struct Object3d
{
	//定数バッファ(行列用)
	ID3D12Resource* constBuffTransform;
	//構造体を変数化
	ConstBufferDataTransform* cbdt = nullptr;
	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;
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
	//定数バッファの設定
	void CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device);
	void CreateConstBufferObject3d(Object3d* object, ID3D12Device* device);
	/// <summary>
	/// 毎フレーム処理
	
	/// </summary>

	//レンダーターゲット設定
	void GetRenderTargetView(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
	//画面クリア設定
	void ClearScreen(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);

	void Update(BYTE* keys);
	void UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matprojection);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList);
	//複数オブジェクト描画用
	void DrawObject3d(Object3d* object,ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vdView,D3D12_INDEX_BUFFER_VIEW& idView,UINT numIndices);
private://メンバ変数
	//頂点数
	static const int VerticesCount_ = 24;
	//インデックス数
	static const int indicesCount_ = 36;
	//オブジェクト数
	static const size_t kObjectCount_ = 50;
	//3Dマテリアル
	Material3d material3d_;
	//3Dオブジェクトの配列
	Object3d object3ds_[kObjectCount_];
	
	//ウィンドゥサイズ
	const int windowWidth = 1280; //横幅
	const int windowHeight = 720; //縦幅

	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT3 normal;	//法線ベクトル
		XMFLOAT2 uv;		//uv座標
	};

	//頂点データ
	Vertex vertices_[VerticesCount_] = {
		//前(Z固定)
		// x	 y	  z		  法線u	  v
		{{-5.0f ,-5.0f ,-5.0f},{},{0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-5.0f,  5.0f ,-5.0f},{},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{ 5.0f ,-5.0f ,-5.0f},{},{1.0f,1.0f}},//右下					
		{{ 5.0f , 5.0f ,-5.0f},{},{1.0f,0.0f}},//右上	
		//後(前面とZ座標の符号が逆)
		// x	 y	  z		 u	  v
		{{-5.0f ,-5.0f ,5.0f},{},{0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-5.0f,  5.0f ,5.0f},{},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{ 5.0f ,-5.0f ,5.0f},{},{1.0f,1.0f}},//右下					
		{{ 5.0f , 5.0f ,5.0f},{}, {1.0f,0.0f}},//右上

		//左(X固定)
		// x	 y	  z		 u	  v
		{{-5.0f ,-5.0f ,-5.0f},{},{0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-5.0f ,5.0f ,-5.0f},{},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{-5.0f ,-5.0f ,5.0f},{},{1.0f,1.0f}},//右下					
		{{-5.0f , 5.0f , 5.0f},{},{1.0f,0.0f}},//右上	
		//右(左面とX座標の符号が逆)
		// x	 y	  z		 u	  v
		{{ 5.0f ,-5.0f ,-5.0f},{}, {0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{ 5.0f ,5.0f , -5.0f},{},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{ 5.0f ,-5.0f ,5.0f},{},{1.0f,1.0f}},//右下					
		{{ 5.0f , 5.0f , 5.0f},{},{1.0f,0.0f}},//右上

		//下(Y固定)
		// x	 y	  z		 u	  v
		{{-5.0f ,-5.0f ,-5.0f},{},{0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-5.0f ,-5.0f , 5.0f},{},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{ 5.0f ,-5.0f ,-5.0f},{},{1.0f,1.0f}},//右下					
		{{ 5.0f ,-5.0f , 5.0f},{},{1.0f,0.0f}},//右上	
		//上(下面とY座標の符号が逆)
		// x	 y	  z		 u	  v
		{{-5.0f , 5.0f ,-5.0f},{},{0.0f,1.0f}},//左下	Xが-で左　Yが-で下
		{{-5.0f , 5.0f , 5.0f},{},{0.0f,0.0f}},//左上	Xが+で右　Yが+で上
		{{ 5.0f , 5.0f ,-5.0f},{},{1.0f,1.0f}},//右下					
		{{ 5.0f , 5.0f , 5.0f},{}, {1.0f,0.0f}},//右上
	};

	//インデックスデータ
	unsigned short indices[indicesCount_] =
	{
		//前
		0,1,2,//三角形1つ目
		2,1,3,//三角形2つ目

		//後(前の面に4加算)
		6,5,4,//三角形3つ目
		7,5,6,//三角形4つ目

		//左
		10,9,8,//三角形5つ目
		10,11,9,//三角形6つ目

		//右
		14,12,13,//三角形7つ目
		15,14,13,//三角形8つ目

		//下
		18,17,16,//三角形9つ目
		19,17,18,//三角形10つ目

		//上
		22,20,21,//三角形11つ目
		22,21,23,//三角形12つ目

	};

	//定数バッファのGPUリソースのポインタ
	ID3D12Resource* constBuffMaterial = nullptr;
	ID3D12Resource* constBuffTransform = nullptr;

	//構造体を変数化
	ConstBufferDataMaterial* cbdm = nullptr;
	ConstBufferDataTransform* cbdt = nullptr;

	//マッピング用のポインタ
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;

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

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW idView{};

	//深度ビュー用のデスクリプタヒープ
	ID3D12DescriptorHeap* dsvHeap = nullptr;

	//設定をもとにSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;

	//パイプラインステートの生成
	ID3D12PipelineState* pipelineState = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	float angle = 0.0f;//カメラの回転角
};
