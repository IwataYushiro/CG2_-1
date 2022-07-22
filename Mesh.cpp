#include "Mesh.h"


Mesh::Mesh()
{

}

Mesh::~Mesh()
{
}

void Mesh::Initialize(HRESULT result, ID3D12Device* device)
{
	for (int i = 0; i < VerticesCount_; i++)
	{
		//三角形一つごとに計算
		//三角形のインデックスを取り出し、一時的な変数に入れる
		unsigned short Index0 = indices[0];
		unsigned short Index1 = indices[1];
		unsigned short Index2 = indices[2];
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

	//定数バッファ生成
	CreateConstBufferMaterial3d(&material3d_, device);
	CreateConstBufferObject3d(&object3d_, device);
	//オブジェクトの初期化
	SetObject3d();

	//ビュープロジェクション初期化
	InitViewProjection();

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

#pragma region 頂点シェーダー

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
	D3D12_ROOT_PARAMETER rootParams[2] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;						//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える
	//定数バッファ1番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[1].Descriptor.ShaderRegister = 1;						//定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える


	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;						//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);						//ルートパラメータ数

	// ルートシグネチャのシリアライズ

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
#pragma endregion
}

void Mesh::CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device)
{
	HRESULT result;

	//定数バッファの設定
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
void Mesh::CreateConstBufferObject3d(Object3d* object, ID3D12Device* device)
{
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

void Mesh::SetObject3d()
{
	//大きさ
	object3d_.scale = { 1.0f,1.0f,1.0f };
	//回転
	object3d_.rotation = { 0.0f,0.0f,0.0f };
	//方向
	object3d_.position = { 0.0f,0.0f,0.0f };
}
void Mesh::InitViewProjection()
{
	//ウィンドゥサイズ
	const int windowWidth = 1280; //横幅
	const int windowHeight = 720; //縦幅

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

}

void Mesh::Update(BYTE* keys, BYTE* preKeys, ID3D12Device* device)
{
	//色が徐々に変わる
	UpdateMaterial3d(&material3d_);
	
//操作
	ControlView(keys);

	UpdateObject3d(&object3d_, matview, matprojection);
	
	ControlObject3d(&object3d_, keys);
}

void Mesh::UpdateMaterial3d(Material3d* material)
{
	//ランダム
	//乱数シードを生成
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジンを生成
	std::mt19937_64 engine(seed_gen());
	//乱数範囲を指定
	std::uniform_real_distribution<float> dist(0.0f, 0.99f);

	red += 1.0f / 255.0f;
	if (red >= 1.0f)
	{
		red = 1.0f;
		green += 1.0f / 255.0f;
	}
	if (green >= 1.0f)
	{
		green = 1.0f / 255.0f;
		blue += dist(engine);
	}
	if (blue >= 1.0f)
	{
		red = 0.0f;
		green = 0.0f;
		blue = dist(engine);
		alpha = dist(engine);
	}

	//値を書き込むと自動的に転送される
	material->constMapMaterial->color = XMFLOAT4(red, green, blue, alpha);

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

	//定数バッファへデータ転送
	object->constMapTransform->mat = object->matWorld * matview * matprojection;
}

void Mesh::ControlObject3d(Object3d* object, BYTE* keys)
{
	if (keys[DIK_UP] || keys[DIK_DOWN] || keys[DIK_RIGHT] || keys[DIK_LEFT])
	{
		if (keys[DIK_UP]) { object->position.y += 1.0f; }
		else if (keys[DIK_DOWN]) { object->position.y -= 1.0f; }
		if (keys[DIK_RIGHT]) { object->position.x += 1.0f; }
		else if (keys[DIK_LEFT]) { object->position.x -= 1.0f; }
	}

	if (keys[DIK_W] || keys[DIK_A] || keys[DIK_S] || keys[DIK_D] || keys[DIK_SPACE])
	{
		if (keys[DIK_W]) { object->rotation.y += XMConvertToRadians(1.0f);}
		else if (keys[DIK_S]) { object->rotation.y -= XMConvertToRadians(1.0f); }
		if (keys[DIK_A]) { object->rotation.x += XMConvertToRadians(1.0f); }
		else if (keys[DIK_D]) { object->rotation.x -= XMConvertToRadians(1.0f); }
		if (keys[DIK_SPACE]) { object->rotation.z -= XMConvertToRadians(1.0f); }
	}
}
void Mesh::ControlView(BYTE* keys)
{
	//視点を操作
	if (keys[DIK_Z] || keys[DIK_X])
	{
		if (keys[DIK_Z]) { angle += XMConvertToRadians(1.0f); }
		else if (keys[DIK_X]) { angle -= XMConvertToRadians(1.0f); }

		//angleラジアンだけy軸周りに回転、半径は-100
		eye.x = -100.0f * sinf(angle);
		eye.z = -100.0f * cosf(angle);

		matview = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
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
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vdView);
	// インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&idView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, material3d_.constBuffMaterial->GetGPUVirtualAddress());

	//描画コマンド
	DrawObject3d(&material3d_,&object3d_, commandList, vdView, idView, _countof(indices));
}

void Mesh::DrawObject3d(Material3d* material,Object3d* object, ID3D12GraphicsCommandList* commandList,
	D3D12_VERTEX_BUFFER_VIEW& vdView, D3D12_INDEX_BUFFER_VIEW& idView, UINT numIndices)
{
	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vdView);
	// インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&idView);
	commandList->SetGraphicsRootConstantBufferView(1, object->constBuffTransform->GetGPUVirtualAddress());

	//インデックスバッファを使う場合
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
	
	//commandList->DrawInstanced(_countof(vertices_), 1, 0, 0);	//全ての頂点を使って描画
}