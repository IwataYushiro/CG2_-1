#include "SpriteCommon.h"
#include <cassert>

SpriteCommon::SpriteCommon()
{
}

SpriteCommon::~SpriteCommon()
{
}

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	HRESULT result{};
	assert(dxCommon);
	this->dxCommon_ = dxCommon;
}
