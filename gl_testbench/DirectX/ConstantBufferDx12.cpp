#include "ConstantBufferDx12.h"
#include <Windows.h>

ConstantBufferDx12::ConstantBufferDx12(std::string NAME, unsigned int location)
{
	this->location	= location;
	this->buff		= nullptr;
	this->index		= 0;
	this->lastMat	= nullptr;
	this->name		= NAME;
	this->handle	= 0;
	this->byteWidth = 0;
}

ConstantBufferDx12::~ConstantBufferDx12()
{
}

void ConstantBufferDx12::setData(const void * data, size_t size, Material * m, unsigned int location)
{
	if (this->byteWidth != size)
	{
		if(this->buff) 
			free(this->buff);

		this->byteWidth = size;
		this->buff = malloc(this->byteWidth);
	}

	memcpy(this->buff, data, this->byteWidth);
}

void ConstantBufferDx12::bind(Material *m)
{
}

void ConstantBufferDx12::init()
{
	
}

void * ConstantBufferDx12::data()
{
	return this->buff;
}

UINT ConstantBufferDx12::size()
{
	return this->byteWidth;
}
