#pragma once
#include <Windows.h>

//入力
class Input
{
public:
	Input();
	~Input();
	
	//初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	
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