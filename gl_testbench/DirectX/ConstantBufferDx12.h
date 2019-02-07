#pragma once
#include "../ConstantBuffer.h"
#include <Windows.h>
#include "D3D12Header.h"

class dxRenderer;
//struct UINT {};

class ConstantBufferDx12 : public ConstantBuffer
{
public:
	ConstantBufferDx12(std::string NAME, unsigned int location);
	ConstantBufferDx12(std::string NAME, unsigned int location, dxRenderer* rnd);
	~ConstantBufferDx12();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);

	void init();
private:
	dxRenderer* rnd;
	std::string name;
	UINT location;
	UINT handle;
	UINT index;
	void* buff = nullptr;
	void* lastMat;

	float4 trans = { 1.0f ,1.0f ,1.0f ,1.0f };
};

