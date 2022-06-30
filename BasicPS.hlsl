#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp      : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	return float4(tex.Sample(smp,input.uv))*color;
	
}

//float4(��,��,��,���l(1)) �͈́@0.0f�`1.0f