#include "Texture2DDx12.h"
#include "Dx12Renderer.h"

//#define STB_IMAGE_IMPLEMENTATION


Texture2DDx12::Texture2DDx12()
{
}

Texture2DDx12::Texture2DDx12(dxRenderer * rnd)
{
	this->rnd = rnd;
}

Texture2DDx12::~Texture2DDx12()
{
}

int Texture2DDx12::loadFromFile(std::string filename)
{
	/*int w, h, bpp;
	unsigned char* rgba = stbi_load(filename.c_str(), &w, &h, &bpp, STBI_rgb_alpha);
	if (rgba == nullptr)
	{
		fprintf(stderr, "Error loading texture file: %s\n", filename.c_str());
		return -1;
	}*/

	/*
	UINT textureAlined = (w * sizeof(DWORD) + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) & ~D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;

	D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = {  };
	pitchedDesc.Format = DXGI_FORMAT_R8G8B8A8_SINT;
	pitchedDesc.Width = w;
	pitchedDesc.Height = h;
	pitchedDesc.Depth = 1;
	pitchedDesc.RowPitch = textureAlined;
	   	

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = { 0 };
	placedTexture2D.Offset = 0;
	placedTexture2D.Footprint = pitchedDesc;

	// NOTE! Width, Height and Array Size 
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	texDesc.Width = 1;
	texDesc.Height = 1;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES hp = {};
	hp.Type = D3D12_HEAP_TYPE_DEFAULT;
	hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //D3D12_CPU_PAGE_PROPERTY_UNKNOWN
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask = 0;
	hp.VisibleNodeMask = 0;


	HRESULT hr = rnd->device4->CreateCommittedResource(
		&hp,
		D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		NULL,
		IID_PPV_ARGS(&resource)
	);
	*/

	/*void* dataBegin = nullptr;
	rnd->textureResource->Map(0, nullptr, &dataBegin);
	memcpy(dataBegin, rgba, w * h);
	rnd->textureResource->Unmap(0, nullptr);*/

	/*D3D12_TEXTURE_COPY_LOCATION tcl = {};
	tcl.pResource = rnd->textureResource;
	tcl.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	tcl.SubresourceIndex = 0;


	rnd->commandAllocator->Reset();
	rnd->commandList4->Reset(rnd->commandAllocator, NULL);

	

	rnd->commandList4->Close();

	ID3D12CommandList* listsToExecute[] = { rnd->commandList4 };
	rnd->commandQueue->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);

	rnd->wait4GPU();*/

	return 1;
}

void Texture2DDx12::bind(unsigned int slot)
{
}
