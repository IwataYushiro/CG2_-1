#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

const float PI = 3.141592f;					//const floatでいい

#pragma comment(lib,"d3dcompiler.lib")

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#pragma region ポインタ置き場
	WinApp* winApp = new WinApp();
	
	DirectXCommon* dxCommon = new DirectXCommon();

	Input* input = new Input();

#pragma endregion

#pragma region Windows初期化
winApp->Initialize();
#pragma endregion
	// DirectX初期化処理　ここから

	//DirectX初期化
	dxCommon->Initialize(winApp);
	// DirectX初期化処理　ここまで

	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	
	input->Initialize(winApp);
	
#pragma endregion
	// 描画初期化処理　ここまで

	//ゲームループ
	while (true)
	{
		//Windowsのメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		// DirectX毎フレーム処理　ここから
		//描画前処理
		dxCommon->PreDraw();

		//描画後処理
		dxCommon->PostDraw();
		// 4.描画コマンドここまで

		// DirectX毎フレーム処理　ここまで

	}
	winApp->Finalize();

	//解放
	delete winApp;
	delete dxCommon;
	delete input;
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}