#include "Mesh.h"
#include <d3dcompiler.h>
#include <string>

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
}

void Mesh::Initialize(HRESULT result, ID3D12Device* device)
{

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

#pragma region グラフィックスパイプライン設定
	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;			//標準設定
	//ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;			//深度クリッピングを有効に
	//ブレンドステートの設定
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//	= D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てのチャンネルを描画
	//ブレンド共通設定(これ＋合成で動く)
	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100％使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//テストの値を0％使う
	//半透明合成(アルファブレンディング　デフォルト)
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	//図形の形状の設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//その他の設定
	pipelineDesc.NumRenderTargets = 1;								//描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;								//1ピクセルにつき1サンプリング

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー
	rootParam.Descriptor.ShaderRegister = 0;						//定数バッファ番号
	rootParam.Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParam;						//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = 1;							//ルートパラメータ数
	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;
#pragma endregion

#pragma region パイプラインステートの生成

	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
#pragma endregion
#pragma endregion
}

void Mesh::Update()
{

}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList)
{

	//全頂点に対して
	for (int i = 0; i < _countof(vertices_); i++)
	{
		vertMap[i] = vertices_[i];		//座標をコピー
	}

	//パイプラインステートとルートシグネチャの設定コマンド
	commandList->SetPipelineState(pipelineState);
	commandList->SetGraphicsRootSignature(rootSignature);

	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vdView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//描画コマンド
	commandList->DrawInstanced(_countof(vertices_), 1, 0, 0);	//全ての頂点を使って描画
}