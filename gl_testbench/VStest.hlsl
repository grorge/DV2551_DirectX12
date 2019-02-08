struct VSIn
{
	float4 pos		: POSITION;
	//float3 color	: COLOR;
};

struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
};
//
//cbuffer CB : register(b0)
//{
//	float R, G, B, A;
//}

cbuffer txBuffer : register(b0)
{
	float R, G, B, A;
}

VSOut VS_main(VSIn input, uint index : SV_VertexID)
{
	VSOut output = (VSOut)0;
	output.pos = float4(input.pos.x + (R),input.pos.y + (G),input.pos.z + (B), 1.0f);
	//output.color = float4(input.color, 1.0f);
	output.color = float4(R, G, B, A);

	return output;
}