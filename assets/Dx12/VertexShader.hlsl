struct VSIn
{
	float4 pos		: POSITION;
	float4 nor		: NORMAL;
	float2 uv		: TEXCOORD;
};

struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 nor		: NORMAL;
	float2 uv		: TEXCOORD;
};

cbuffer CB : register(b0)
{
	float R, G, B, A;
}

VSOut main(
	float4 position : POSITION,
	float4 normal	: NORMAL,
	float2 uv		: TEXCOORD,
	uint index		: SV_VertexID,
	uint instance	: SV_InstanceID)
{
	VSOut output = (VSOut)0;
	output.pos	= position + index * instance;
	output.nor	= normal + index * instance;
	output.uv	= uv + index * instance;
	//output.color = float4(R, G, B, A);

	return output;
}