#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

///<summary>
//初期化
///<summary>
void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

#pragma	region DirectInputの初期化
	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion
}

///<summary>
//更新
///<summary>
void Input::Update()
{
	//キーボード情報の取得開始
	keyboard->Acquire();

	//全キーの入力状態を取得する
	
	for (int i = 0; i < 256; ++i)
	{
		preKeys[i] = keys[i];
	}

	keyboard->GetDeviceState(sizeof(keys), keys);

}
///<summary>
///キーが押されているか
///<summary>
bool Input::PushKey(BYTE keyNumber) {

	//指定キーを押していればtrue
	if (keys[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}