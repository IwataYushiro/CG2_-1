#include "Mesh.h"
#include <d3dcompiler.h>
#include <string>
#include <math.h>
#include <DirectXTex.h>

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
}

void Mesh::Initialize(HRESULT result, ID3D12Device* device)
{
#pragma region 描画初期化処理
	for (int i = 0; i < VerticesCount_; i++)
	{
		//三角形一つごとに計算
		//三角形のインデックスを取り出し、一時的な変数に入れる
		unsigned short Index0 = indices[i * 3 + 0];
		unsigned short Index1 = indices[i * 3 + 1];
		unsigned short Index2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices_[Index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices_[Index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices_[Index2].pos);
		//p0=>p1,p0=>p2ベクトルを計算(ベクトル減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化(長さを1に)
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices_[Index0].normal, normal);
		XMStoreFloat3(&vertices_[Index1].normal, normal);
		XMStoreFloat3(&vertices_[Index2].normal, normal);
	}
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));

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
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);


	//定数バッファの設定
	CreateConstBufferMaterial3d(&material3d_, device);

	//オブジェクトの初期化
	for (int i = 0; i < _countof(object3ds_); i++)
	{
		CreateConstBufferObject3d(&object3ds_[i], device);
		//以下親子構造のサンプル
		SetObject3ds(i);
	}

	//値を書き込むと自動的に転送される
	material3d_.constMapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//単位行列を代入
	//平行投影変換
	//constMapTransform->mat = XMMatrixOrthographicOffCenterLH(0, windowWidth, windowHeight, 0, 0, 1);

	//透視投影変換行列の計算
	matprojection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)windowWidth / windowHeight,
		0.1f, 1000.0f
	);

	//ビュー変換行列

	//視点
	eye = { 0.0f, 0.0f, -100.0f };
	//注視点
	target = { 0.0f, 0.0f, 0.0f };
	//上方向ベクトル
	up = { 0.0f, 1.0f, 0.0f };

	matview = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	this->isSpace = false;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/texture.png",	//Resourcesフォルダのtexture.png
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	//テクスチャバッファ設定

	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;							//幅
	textureResourceDesc.Height = (UINT16)metadata.height;				//高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	ComPtr<ID3D12Resource> texbuff = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));
	assert(SUCCEEDED(result));



	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texbuff->WriteToSubresource(
			(UINT)i,
			nullptr,							//全領域へコピー
			img->pixels,						//元データアドレス
			(UINT)img->rowPitch,				//1ラインサイズ
			(UINT)img->slicePitch				//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	TexMetadata metadata2{};
	ScratchImage scratchImg2{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/reimu.png",	//Resourcesフォルダのtexture.png
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);

	assert(SUCCEEDED(result));

	ScratchImage mipChain2{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImg2.GetImages(), scratchImg2.GetImageCount(), scratchImg2.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain2);
	if (SUCCEEDED(result))
	{
		scratchImg2 = std::move(mipChain2);
		metadata2 = scratchImg2.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata2.format = MakeSRGB(metadata2.format);

	//テクスチャバッファ設定
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;							//幅
	textureResourceDesc2.Height = (UINT16)metadata2.height;				//高さ
	textureResourceDesc2.DepthOrArraySize = (UINT16)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT16)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	ComPtr<ID3D12Resource> texbuff2 = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff2));
	assert(SUCCEEDED(result));



	//全ミップマップについて
	for (size_t i = 0; i < metadata2.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img2 = scratchImg2.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texbuff2->WriteToSubresource(
			(UINT)i,
			nullptr,							//全領域へコピー
			img2->pixels,						//元データアドレス
			(UINT)img2->rowPitch,				//1ラインサイズ
			(UINT)img2->slicePitch				//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}
	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定をもとにSRV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	//SRVヒープの戦闘ハンドルを取得
	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};				//設定構造体
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device->CreateShaderResourceView(texbuff.Get(), &srvDesc, srvHandle);
	//1つハンドルを進める
	incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};				//設定構造体
	srvDesc2.Format = resDesc.Format;
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device->CreateShaderResourceView(texbuff2.Get(), &srvDesc2, srvHandle);


	//深度バッファ設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = windowWidth;		//レンダーターゲットに合わせる
	depthResourceDesc.Height = windowHeight;	//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//深度バッファ生成
	ComPtr<ID3D12Resource> depthBuff = nullptr;
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに必要
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));

	//深度ビュー用デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //デプスステンシルビュー

	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));


	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	// GPU仮想アドレス
	vdView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vdView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vdView.StrideInBytes = sizeof(vertices_[0]);

	//インデックスバッファビューの作成
	idView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	idView.Format = DXGI_FORMAT_R16_UINT;
	idView.SizeInBytes = sizeIB;

	//深度ビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

#pragma region 頂点シェーダー
	ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
				   
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
		//xyz座標(1行で書いた方が見やすい)
		{
			"POSITION",										//セマンティック名
			0,												//同じセマンティック名が複数あるときに使うインデックス(0でいい)
			DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す(XYZの3つでfloat型なのでR32G32B32_FLOAT)
			0,												//入力スロットインデックス(0でいい)
			D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット値(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
			0												//一度に描画するインスタンス数(0でいい)
		},//座標以外に色、テクスチャUV等を渡す場合はさらに続ける
		//法線ベクトル		
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//uv座標(1行で書いた方が見やすい)
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,						//uvはfloatが二つなのでフォーマットはRGだけ
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}


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
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	//カリングする(しないときはBACKをNONEにする)
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
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0〜255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;								//1ピクセルにつき1サンプリング

	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;							//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; //書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;								//深度値フォーマット

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;								//一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;							//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[0].Descriptor.ShaderRegister = 0;						//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[2].Descriptor.ShaderRegister = 1;						//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//横繰り返し(タイリング)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//縦繰り返し(タイリング)
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//奥行繰り返し(タイリング)
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//ピクセルシェーダーからのみ使用可能

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;						//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);			//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();
#pragma endregion

#pragma region パイプラインステートの生成

	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
#pragma endregion
}

void Mesh::CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device) {

	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(material->cbdm) + 0xff) & ~0xff;//256バイトアラインメント
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
		IID_PPV_ARGS(&material->constBuffMaterial));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = material->constBuffMaterial->Map(0, nullptr, (void**)&material->constMapMaterial);//マッピング
	assert(SUCCEEDED(result));
}

void Mesh::CreateConstBufferObject3d(Object3d* object, ID3D12Device* device) {

	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(object->cbdt) + 0xff) & ~0xff;//256バイトアラインメント
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
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform);//マッピング
	assert(SUCCEEDED(result));
}
//オブジェクトの初期化
void Mesh::SetObject3ds(int num)
{
	//乱数シードを生成
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジンを生成
	std::mt19937_64 engine(seed_gen());
	//乱数範囲を指定
	std::uniform_real_distribution<float> dist(-25.0f, 25.0f);

	//乱数エンジンを変数に渡す
	float randPosX = dist(engine);
	float randPosY = dist(engine);
	float randPosZ = dist(engine);
	//先頭以外なら
	if (num > 0)
	{
		//1つ前のオブジェクトを親オブジェクトにする//これがないと追従しなくなる
		//object3ds_[i].parent = &object3ds_[i - 1];
		//親オブジェクトの9割の大きさ
		object3ds_[num].scale = { 0.5f,0.5f,0.5f };
		//親オブジェクトに対してZ軸周りに30度回転
		object3ds_[num].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };

		//親オブジェクトに対してZ軸方向に-8.0fずらす
		object3ds_[num].position = { randPosX,randPosY,randPosZ };
	}
}

void Mesh::CreateTextureBuffer(ID3D12Device* device, D3D12_RESOURCE_DESC resDesc)
{
	HRESULT result;

	for (int i = 0; i < textureCount; i++)
	{
		TexMetadata* metadata{};
		ScratchImage* scratchImg{};
		//WICテクスチャのロード
		if (i == 0)
		{
			result = LoadFromWICFile(
				L"Resources/texture.png",	//Resourcesフォルダのtexture.png
				WIC_FLAGS_NONE,
				&metadata[0], scratchImg[0]);
		}
		else if (i == 1)
		{
			result = LoadFromWICFile(
				L"Resources/reimu.png",	//Resourcesフォルダのtexture.png
				WIC_FLAGS_NONE,
				&metadata[1], scratchImg[1]);
		}

		assert(SUCCEEDED(result));

		ScratchImage* mipChain{};
		//ミップマップ生成
		result = GenerateMipMaps(
			scratchImg[i].GetImages(), scratchImg[i].GetImageCount(), scratchImg[i].GetMetadata(),
			TEX_FILTER_DEFAULT, 0, mipChain[i]);
		if (SUCCEEDED(result))
		{
			scratchImg[i] = std::move(mipChain[i]);
			metadata[i] = scratchImg[i].GetMetadata();
		}
		//読み込んだディフューズテクスチャをSRGBとして扱う
		metadata[i].format = MakeSRGB(metadata[i].format);

		//テクスチャバッファ設定

		// ヒープ設定
		D3D12_HEAP_PROPERTIES textureHeapProp{};
		textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		textureHeapProp.CPUPageProperty =
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		//リソース設定
		D3D12_RESOURCE_DESC* textureResourceDesc{};
		textureResourceDesc[i].Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureResourceDesc[i].Format = metadata[i].format;
		textureResourceDesc[i].Width = metadata[i].width;							//幅
		textureResourceDesc[i].Height = (UINT16)metadata[i].height;				//高さ
		textureResourceDesc[i].DepthOrArraySize = (UINT16)metadata[i].arraySize;
		textureResourceDesc[i].MipLevels = (UINT16)metadata[i].mipLevels;
		textureResourceDesc[i].SampleDesc.Count = 1;

		//テクスチャバッファの生成
		ID3D12Resource** texbuff = nullptr;
		result = device->CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc[i],
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texbuff[i]));
		assert(SUCCEEDED(result));

		//全ミップマップについて
		for (size_t j = 0; j < metadata[i].mipLevels; j++)
		{
			//ミップマップレベルを指定してイメージを取得
			const Image* img = scratchImg[i].GetImage(j, 0, 0);
			//テクスチャバッファにデータ転送
			result = texbuff[j]->WriteToSubresource(
				(UINT)j,
				nullptr,							//全領域へコピー
				img->pixels,						//元データアドレス
				(UINT)img->rowPitch,				//1ラインサイズ
				(UINT)img->slicePitch				//1枚サイズ
			);
			assert(SUCCEEDED(result));
		}


		//SRVの最大個数
		const size_t kMaxSRVCount = 2056;

		//デスクリプタヒープ設定
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
		srvHeapDesc.NumDescriptors = kMaxSRVCount;

		//設定をもとにSRV用デスクリプタヒープを生成
		result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
		assert(SUCCEEDED(result));

		//SRVヒープの戦闘ハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
		if (i != textureCount - 1)
		{
			//1つハンドルを進める
			incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			srvHandle.ptr += incrementSize;
		}

		//シェーダーリソースビュー設定
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc{};				//設定構造体
		srvDesc[i].Format = textureResourceDesc[i].Format;
		srvDesc[i].Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc[i].ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
		srvDesc[i].Texture2D.MipLevels = textureResourceDesc[i].MipLevels;

		//ハンドルの指す位置にシェーダーリソースビュー作成
		device->CreateShaderResourceView(texbuff[i], &srvDesc[i], srvHandle);
	}
}

void Mesh::GetRenderTargetView(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle)
{
	// レンダーターゲットビューのハンドルを取得
	dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

}

void Mesh::ClearScreen(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle)
{
	// 3.画面クリア			R	  G		B	A

	FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f }; //青っぽい色
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Mesh::Update(BYTE* keys, BYTE* preKeys, ID3D12Device* device)
{
	//視点を操作
	if (keys[DIK_D] || keys[DIK_A])
	{
		if (keys[DIK_D]) { angle += XMConvertToRadians(1.0f); }
		else if (keys[DIK_A]) { angle -= XMConvertToRadians(1.0f); }

		//angleラジアンだけy軸周りに回転、半径は-100
		eye.x = -100.0f * sinf(angle);
		eye.z = -100.0f * cosf(angle);

		matview = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	}
	if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
	{
		if (!isSpace)
		{
			isSpace = true;
			srvHandle.ptr += incrementSize;
			srvGpuHandle.ptr += incrementSize;
		}
		else if(isSpace)
		{
			isSpace = false;
			srvHandle.ptr -= incrementSize;
			srvGpuHandle.ptr -= incrementSize;
		}

	}
	for (int i = 0; i < _countof(object3ds_); i++)
	{
		UpdateObject3d(&object3ds_[i], matview, matprojection);
	}
	//操作
	ControlObject3d(&object3ds_[0], keys);
}

void Mesh::UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matprojection)
{
	XMMATRIX matScale, matRot, matTrans;

	//スケーリング等計算
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);

	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);
	//ワールド行列合成
	object->matWorld = XMMatrixIdentity();
	object->matWorld *= matScale;	//スケーリング反映
	object->matWorld *= matRot;		//回転反映
	object->matWorld *= matTrans;	//平行移動反映

	//親オブジェクトがあれば
	if (object->parent != nullptr)
	{
		//親オブジェクトのワールド行列を掛け算
		object->matWorld *= object->parent->matWorld;
	}

	//定数バッファへデータ転送
	object->constMapTransform->mat = object->matWorld * matview * matprojection;
}

//操作
void Mesh::ControlObject3d(Object3d* object, BYTE* keys)
{
	if (keys[DIK_UP] || keys[DIK_DOWN] || keys[DIK_RIGHT] || keys[DIK_LEFT])
	{
		if (keys[DIK_UP]) { object->position.y += 1.0f; }
		else if (keys[DIK_DOWN]) { object->position.y -= 1.0f; }
		if (keys[DIK_RIGHT]) { object->position.x += 1.0f; }
		else if (keys[DIK_LEFT]) { object->position.x -= 1.0f; }
	}
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList)
{
	//全頂点に対して
	for (int i = 0; i < _countof(vertices_); i++)
	{
		vertMap[i] = vertices_[i];		//座標をコピー
	}

	//パイプラインステートとルートシグネチャの設定コマンド
	commandList->SetPipelineState(pipelineState.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	//プリミティブ形状の設定コマンド
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//リストかストリップか
	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vdView);
	// インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&idView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, material3d_.constBuffMaterial->GetGPUVirtualAddress());
	//SRVヒープの設定コマンド
	commandList->SetDescriptorHeaps(1, &srvHeap);
	// SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
	
	// SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	//全オブジェクト描画
	for (int i = 0; i < _countof(object3ds_); i++)
	{
		DrawObject3d(&object3ds_[i], commandList, vdView, idView, _countof(indices));
	}
	//描画コマンド
	//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);	//全ての頂点を使って描画
}

void Mesh::DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList,
	D3D12_VERTEX_BUFFER_VIEW& vdView, D3D12_INDEX_BUFFER_VIEW& idView, UINT numIndices)
{
	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vdView);
	// インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&idView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());
	//インデックスバッファを使う場合
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}
