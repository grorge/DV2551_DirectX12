#pragma once
#include "../ConstantBuffer.h"

struct UINT {};

class ConstantBufferDx12 : public ConstantBuffer
{
public:
	ConstantBufferDx12(std::string NAME, unsigned int location);
	~ConstantBufferDx12();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);

private:

	std::string name;
	UINT location;
	UINT handle;
	UINT index;
	void* buff = nullptr;
	void* lastMat;
};

