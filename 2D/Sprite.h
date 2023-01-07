#pragma once
#include "SpriteCommon.h"
#include <DirectXMath.h>

//1枚1枚のスプライトクラス

class Sprite
{
public: // メンバ関数
	//エイリアステンプレートとかで色々省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://メンバ関数

	//初期化
	void Initialize(SpriteCommon* spCommon);

	//描画
	void Draw();

private://メンバ変数
	//頂点数
	static const size_t verticesCount = 3;

	//頂点データ
	XMFLOAT3 vertices[verticesCount] = {
		{-0.5f,-0.5f,0.0f},
		{-0.5f,+0.5f,0.0f},
		{+0.5f,-0.5f,0.0f},
	};

	SpriteCommon* spCommon_ = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	
};
