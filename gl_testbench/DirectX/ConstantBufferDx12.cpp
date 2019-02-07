#include "ConstantBufferDx12.h"
#include <Windows.h>
#include "Dx12Renderer.h"

ConstantBufferDx12::ConstantBufferDx12(std::string NAME, unsigned int location)
{
	this->location	= location;
	this->buff		= nullptr;
	this->index		= 0;
	this->lastMat	= nullptr;
	this->name		= NAME;
	this->handle	= 0;
}

ConstantBufferDx12::ConstantBufferDx12(std::string NAME, unsigned int location, dxRenderer * rnd)
{
	this->location = location;
	this->buff = nullptr;
	this->index = 0;
	this->lastMat = nullptr;
	this->name = NAME;
	this->handle = 0;
	this->rnd = rnd;

	
}

ConstantBufferDx12::~ConstantBufferDx12()
{
}

void ConstantBufferDx12::setData(const void * data, size_t size, Material * m, unsigned int location)
{
	UINT backBufferIndex = this->rnd->swapChain4->GetCurrentBackBufferIndex();
	
	//this->trans = (float4*)data;

	float4* temp = (float4*)data;
	this->trans.x = temp->x;
	this->trans.y = temp->y;
	this->trans.z = temp->z;
	this->trans.w = temp->w;

	//Update GPU memory
	void* mappedMem = nullptr;
	D3D12_RANGE readRange = { 0, 0 }; //We do not intend to read this resource on the CPU.
	if (SUCCEEDED(this->rnd->constantBufferResource[backBufferIndex]->Map(0, &readRange, &mappedMem)))
	{
		//memcpy(mappedMem, &data, size);
		memcpy(mappedMem, &this->trans, size);

		//D3D12_RANGE writeRange = { 0, sizeof(float) * 4 };
		D3D12_RANGE writeRange = { 0, size};
		this->rnd->constantBufferResource[backBufferIndex]->Unmap(0, &writeRange);
	}
}

void ConstantBufferDx12::bind(Material *)
{
}

void ConstantBufferDx12::init()
{
	
}
