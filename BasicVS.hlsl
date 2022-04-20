float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}
//　return pos + float4(<-Xオフセット+>,Yオフセット,0,0) 指定しただけずれる
//　return pos * float4(X拡長, Y拡張, Y縮小, Y縮小) 指定しただけ拡縮される