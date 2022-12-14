#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>

//スプライト基盤クラス

class SpriteCommon
{
public: 
	//エイリアステンプレートとかで色々省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//描画前処理
	void PreDraw();

private://メンバ変数

	DirectXCommon* dxCommon_ = nullptr;

	
	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState;
//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

public://アクセッサ

	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
