cbuffer ConstBufferDataMaterial:register(b0)
{
	float4 color;
}
float4 main() : SV_TARGET
{
	return color;
}

//float4(Ô,—Î,Â,ƒ¿’l(1)) ”ÍˆÍ@0.0f`1.0f