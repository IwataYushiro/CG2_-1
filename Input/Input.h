#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInputのバージョン指定

#include <dinput.h>
#include <Windows.h>
#include <wrl.h>

//入力
class Input
{
public:
	//エイリアステンプレートでnamespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Input();
	~Input();
	
	//初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	
	//更新
	void Update();

private://メンバ変数
	ComPtr <IDirectInputDevice8> keyboard = nullptr;

};

Input::Input()
{
}

Input::~Input()
{
}