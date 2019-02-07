#pragma once

#include "../VertexBuffer.h"
#include "D3D12Header.h"

class dxRenderer;

class VertexBufferDx12 : public VertexBuffer
{

public:

	VertexBufferDx12(size_t size, VertexBuffer::DATA_USAGE usage);
	VertexBufferDx12(
		size_t size, 
		VertexBuffer::DATA_USAGE usage, 
		dxRenderer* rnd);
	~VertexBufferDx12();

	void setData(const void* data, size_t size, size_t offset);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();

	static UINT usageMapping[3];

private:
	size_t totalSize;
	UINT _handle;
	dxRenderer* rnd;

	ID3D12Resource * resource;
	D3D12_VERTEX_BUFFER_VIEW resourceView;
};