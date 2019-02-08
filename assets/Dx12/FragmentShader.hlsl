struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 nor		: NORMAL;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

float4 main(VSOut input) : SV_TARGET0
{
	//return float4(1.0f, 0.0f, 1.0f, 1.0f);
	return input.color;
}