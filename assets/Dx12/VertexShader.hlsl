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
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

cbuffer descriptorHeapConstantBuffer1 : register (b0)
{
	float4 translate[512];	// buffer 1
}

cbuffer descriptorHeapConstantBuffer2 : register (b1)
{
	float4 color[512];	// buffer 1
}

VSOut main(
	float4 position : POSITION,
	float4 normal	: NORMAL,
	float2 uv		: TEXCOORD,
	uint vertex		: SV_VertexID,
	uint instance	: SV_InstanceID)
{
	VSOut output	= (VSOut)0;
	output.pos		= position + translate[instance];
	output.nor		= normal;
	output.uv		= uv;
	output.color	= color[instance];

	return output;
}