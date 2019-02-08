#pragma once
#include "../ConstantBuffer.h"
#include <Windows.h>


class ConstantBufferDx12 : public ConstantBuffer
{
public:
	ConstantBufferDx12(std::string NAME, unsigned int location);
	~ConstantBufferDx12();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material *m);

	void init();

	void* data();
	UINT size();

private:

	std::string name;
	UINT location;
	UINT handle;
	UINT index;
	void* buff = nullptr;
	void* lastMat;
	UINT byteWidth;
};

