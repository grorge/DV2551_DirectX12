#include "Sampler2DDx12.h"
#include "Dx12Renderer.h"

Sampler2DDx12::Sampler2DDx12()
{
	this->samplerDesc = {};
	this->cpuHandler = { 0 };
	this->rnd = nullptr;
}

Sampler2DDx12::Sampler2DDx12(dxRenderer * rnd, int location)
{
	this->samplerDesc = {};
	this->cpuHandler = { 0 };
	this->rnd = rnd;

	this->samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	this->cpuHandler = rnd->descriptorHeapSampler->GetCPUDescriptorHandleForHeapStart();
	UINT samplerSize = rnd->device4->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	this->cpuHandler.ptr += samplerSize * location;
	rnd->device4->CreateSampler(&samplerDesc, this->cpuHandler);
}

Sampler2DDx12::~Sampler2DDx12()
{
}

void Sampler2DDx12::setMagFilter(FILTER filter)
{

}

void Sampler2DDx12::setMinFilter(FILTER filter)
{

}

void Sampler2DDx12::setWrap(WRAPPING s, WRAPPING t)
{

}
