#pragma once
#define DIRECTINPUT_VERSION		0x0800		//DirectInputのバージョン指定
#include <cassert>
#include <dinput.h>
#include <wrl.h>

using namespace Microsoft::WRL;

//入力
class Input
{
public:
	Input();
	~Input();
	
	//初期化
	void Initialize();
	
	//更新
	void Update();
private:

};

Input::Input()
{
}

Input::~Input()
{
}