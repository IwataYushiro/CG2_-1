#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//ウィンドゥプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドゥが破棄された
	case WM_DESTROY:
		//OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドゥサイズ
	const int window_width = 1280; //横幅
	const int window_height = 720; //縦幅

	//ウィンドゥクラスの設定
	WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;      //ウィンドゥプロシージャを設定 
	w.lpszClassName = L"DX12Sample";           //ウィンドゥクラス名
	w.hInstance = GetModuleHandle(nullptr);   //ウィンドゥハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);  //カーソル指定

	//ウィンドゥクラスをOSに登録
	RegisterClassEx(&w);
	//ウィンドウサイズ {X座標,Y座標,横幅,縦幅}
	RECT wrc = { 0,0,window_width,window_height };
	//自動でサイズを補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(w.lpszClassName,   //クラス名
		L"DirectXGame",							//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,					//標準的なウィンドウスタイル
		CW_USEDEFAULT,							//表示X座標(OSに任せる)
		CW_USEDEFAULT,							//表示Y座標(OSに任せる)
		wrc.right - wrc.left,					//ウィンドウ横幅
		wrc.bottom - wrc.top,					//ウィンドウ縦幅
		nullptr,								//親ウィンドウハンドル
		nullptr,								//メニューハンドル
		w.hInstance,							//呼び出しアプリケーションハンドル
		nullptr);								//オプション

	//ウィンドゥを表示状態にする
	ShowWindow(hwnd, SW_SHOW);

	MSG msg{}; //メッセージ

	// DirectX初期化処理　ここから

	// DirectX初期化処理　ここまで

	//ゲームループ
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

	// DirectX毎フレーム処理　ここから

	// DirectX毎フレーム処理　ここまで
	
	}

	//ウィンドゥクラスを登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}