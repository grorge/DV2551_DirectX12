#define SHADER_ID 0

struct VSIn
{
	float4 pos		: POSITION0;
	float4 nor		: NORMAL0;
	float2 uv		: TEXCOORD0;
};

struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 nor		: NORMAL0;
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

cbuffer descriptorHeapConstantBuffer1 : register (b0)
{
	float4 translate[512];
}

cbuffer descriptorHeapConstantBuffer2 : register (b1)
{
	float4 color[512];
}

VSOut main(
	float4 position : POSITION0,
	float4 normal	: NORMAL0,
	float2 uv		: TEXCOORD0,
	uint vertex		: SV_VertexID,
	uint instance	: SV_InstanceID)
{
	VSOut output	= (VSOut)0;

	output.pos		= position + translate[instance + (SHADER_ID * 25)];
	output.nor		= normal;
	output.uv		= uv;
	output.color	= color[instance];

	return output;
}