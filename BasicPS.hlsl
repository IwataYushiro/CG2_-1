#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp      : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(tex.Sample(smp,input.uv))*color;
	float3 light = normalize(float3(1.0f,-1.0f,1.0f));		//右下奥　向きのライト
	float brightness = dot(-light, input.normal);			//光源へのベクトルと法線ベクトルの内積
	return float4(brightness, brightness, brightness, 1);	//輝度をRGBに代入して出力
}

//float4(赤,緑,青,α値(1)) 範囲　0.0f～1.0f