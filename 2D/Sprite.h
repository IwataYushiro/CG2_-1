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
	Sprite();
	~Sprite();

	//初期化
	void Initialize(SpriteCommon* spCommon);

private://メンバ変数

	SpriteCommon* spCommon_ = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertbuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	
};
