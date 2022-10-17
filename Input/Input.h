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
	
	///<summary>
	//初期化
	///<summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	
	///<summary>
	//更新
	///<summary>
	void Update();

	///<summary>
	///キーが押されているか
	///<summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

private://メンバ変数
	//キーボード
	ComPtr <IDirectInputDevice8> keyboard = nullptr;
	
	//全キーの状態
	BYTE preKeys[256] = {};
	BYTE keys[256] = {};

};

Input::Input()
{
}

Input::~Input()
{
}