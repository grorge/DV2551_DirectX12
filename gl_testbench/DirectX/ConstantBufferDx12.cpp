#include "ConstantBufferDx12.h"
#include <Windows.h>

ConstantBufferDx12::ConstantBufferDx12(std::string NAME, unsigned int location)
{
	this->location	= 0;
	this->buff		= nullptr;
	this->index		= 0;
	this->lastMat	= nullptr;
	this->name		= "Unknown";
	this->handle	= 0;
}

ConstantBufferDx12::~ConstantBufferDx12()
{
}

void ConstantBufferDx12::setData(const void * data, size_t size, Material * m, unsigned int location)
{
}

void ConstantBufferDx12::bind(Material *)
{
}
