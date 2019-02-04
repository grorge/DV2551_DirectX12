#include "Texture2DDx12.h"
#include "Dx12Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2DDx12::Texture2DDx12()
{
}

Texture2DDx12::~Texture2DDx12()
{
}

int Texture2DDx12::loadFromFile(std::string filename)
{
	int w, h, bpp;
	unsigned char* rgb = stbi_load(filename.c_str(), &w, &h, &bpp, STBI_rgb_alpha);
	if (rgb == nullptr)
	{
		fprintf(stderr, "Error loading texture file: %s\n", filename.c_str());
		return -1;
	}

	UINT textureAlined = (w * sizeof(DWORD) + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) & ~D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;

	D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = {  };
	pitchedDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	pitchedDesc.Width = w;
	pitchedDesc.Height = h;
	pitchedDesc.Depth = 1;
	pitchedDesc.RowPitch = textureAlined;
	   	

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = { 0 };
	placedTexture2D.Offset = 0;
	placedTexture2D.Footprint = pitchedDesc;


	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = 2;

	return 0;
}

void Texture2DDx12::bind(unsigned int slot)
{
}
