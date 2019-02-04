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

	D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = { 0 };
	pitchedDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	pitchedDesc.Width = bitmapWidth;
	pitchedDesc.Height = bitmapHeight;
	pitchedDesc.Depth = 1;
	pitchedDesc.RowPitch = Align(bitmapWidth * sizeof(DWORD), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);



	return 0;
}

void Texture2DDx12::bind(unsigned int slot)
{
}
