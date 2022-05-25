#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::Initialize()
{
	XMFLOAT3 vertices[] = {
		{-0.5f,-0.5f,0.0f},//左下	Xが-で左　Yが-で下
		{-0.5f,+0.5f,0.0f},//左上	Xが-で左　Yが+で上
		{+0.5f,-0.5f,0.0f},//右下	Xが+で右　Yが-で下
	};
}

void Mesh::Update()
{

}

void Mesh::Draw()
{

}