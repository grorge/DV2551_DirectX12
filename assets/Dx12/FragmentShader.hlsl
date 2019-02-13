struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 nor		: NORMAL0;
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

SamplerState samplerState : register (s0);
Texture2D texture2d : register (t0);

float4 main(VSOut input) : SV_TARGET0
{
	float4 color = 1.0f;

#ifdef DIFFUSE_SLOT
	color = /*float4(1.0f, 0.0f, 1.0f, 1.0f);*/ texture2d.Sample(samplerState, input.uv);
#else
	color = input.color;
#endif

	return color;
}