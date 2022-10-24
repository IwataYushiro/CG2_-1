#pragma once
#include <Windows.h>

class WinApp
{
public://定数
	//ウィンドゥサイズ
	static const int window_width = 1280; //横幅
	static const int window_height = 720; //縦幅

public://メンバ関数
	WinApp();
	~WinApp();

	//ウィンドゥプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//初期化
	void Initialize();

	//更新
	void Update();

private://メンバ変数
	//ウィンドゥハンドル
	HWND hwnd = nullptr;

public://アクセッサ置き場
	HWND GetHwnd() const { return hwnd; }
};

WinApp::WinApp()
{
}

WinApp::~WinApp()
{
}