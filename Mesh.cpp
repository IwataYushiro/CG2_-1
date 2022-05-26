#include "Mesh.h"
#include <d3dcompiler.h>
#include <string>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::Initialize()
{
	XMFLOAT3 vertices_[][3] = {
		{
		{-0.5f,-0.5f,0.0f},//左下	Xが-で左　Yが-で下
		{-0.5f,+0.5f,0.0f},//左上	Xが-で左　Yが+で上
		{+0.5f,-0.5f,0.0f},//右下	Xが+で右　Yが-で下
		},
		{
		{-0.7f,-0.5f,0.0f},//左下	Xが-で左　Yが-で下
		{-0.6f,+0.5f,0.0f},//左上	Xが-で左　Yが+で上
		{-0.7f,-0.5f,0.0f},//右下	Xが+で右　Yが-で下
		},
	};
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices_));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	
	//頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));
	//定数バッファの設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256バイトアラインメント
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));


		//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(result));
	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// GPU仮想アドレス
	vdView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vdView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vdView.StrideInBytes = sizeof(XMFLOAT3);

#pragma region 頂点シェーダー
	ID3DBlob* vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;	//エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",								//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				//インクルード可能にする
		"main", "vs_5_0",								//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
		0,
		&vsBlob, &errorBlob);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlodからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize()),

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				error.begin());
		error += "\n";
		//エラー内容を出力ウィンドゥに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
#pragma endregion

#pragma region ピクセルシェーダー
	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",								//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				//インクルード可能にする
		"main", "ps_5_0",								//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
		0,
		&psBlob, &errorBlob);
	//エラーなら
	if (FAILED(result))
	{
		//errorBlodからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize()),

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				error.begin());
		error += "\n";
		//エラー内容を出力ウィンドゥに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
#pragma endregion

#pragma region 頂点レイアウト
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",										//セマンティック名
			0,												//同じセマンティック名が複数あるときに使うインデックス(0でいい)
			DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す(XYZの3つでfloat型なのでR32G32B32_FLOAT)
			0,												//入力スロットインデックス(0でいい)
			D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット値(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
			0												//一度に描画するインスタンス数(0でいい)
		},//(1行で書いた方が見やすい)
		//座標以外に色、テクスチャUV等を渡す場合はさらに続ける
	};
#pragma endregion

#pragma endregion
}

void Mesh::Update()
{

}

void Mesh::Draw()
{
	
	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vdView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//描画コマンド
	commandList->DrawInstanced(_countof(vertices_), 1, 0, 0);	//全ての頂点を使って描画
}