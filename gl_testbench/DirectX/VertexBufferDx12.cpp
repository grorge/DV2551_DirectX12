#include "VertexBufferDx12.h"
#include "Dx12Renderer.h"

VertexBufferDx12::VertexBufferDx12(size_t size, VertexBuffer::DATA_USAGE usage)
{

}

VertexBufferDx12::VertexBufferDx12(
	size_t size, 
	size_t totSize, 
	dxRenderer * rnd)
{
	this->rnd = rnd;
	this->totalSize = totSize;

	//Note: using upload heaps to transfer static data like vert buffers is not 
	//recommended. Every time the GPU needs it, the upload heap will be marshalled 
	//over. Please read up on Default Heap usage. An upload heap is used here for 
	//code simplicity and because there are very few vertices to actually transfer.
	D3D12_HEAP_PROPERTIES hp = {};
	hp.Type = D3D12_HEAP_TYPE_UPLOAD;
	hp.CreationNodeMask = 1;
	hp.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC rd = {};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd.Width			= totSize;
	rd.Height			= 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels		= 1;
	rd.SampleDesc.Count = 1;
	rd.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Creates both a resource and an implicit heap, such that the heap is big enough
	//to contain the entire resource and the resource is mapped to the heap. 
	HRESULT hr = this->rnd->device4->CreateCommittedResource(
		&hp,
		D3D12_HEAP_FLAG_NONE,
		&rd,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&this->resource));

	this->resource->SetName(L"vb heap");

	resourceView.BufferLocation	= this->resource->GetGPUVirtualAddress();
	resourceView.SizeInBytes		= totSize;
	resourceView.StrideInBytes		= size;


	//hp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//hp.CreationNodeMask = 1;
	//hp.VisibleNodeMask = 1;

	//D3D12_RESOURCE_DESC rd = {};
	//rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//rd.Width			= this->totalSize;
	//rd.Height			= 1;
	//rd.DepthOrArraySize = 1;
	//rd.MipLevels		= 1;
	//rd.SampleDesc.Count = 1;
	//rd.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	////Creates both a resource and an implicit heap, such that the heap is big enough
	////to contain the entire resource and the resource is mapped to the heap. 
	//HRESULT hr = this->rnd->device4->CreateCommittedResource(
	//	&hp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&rd,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&this->resource));

	//this->resource->SetName(L"vb heap"); 

	//this->resourceView.SizeInBytes		= this->totalSize;
	//this->resourceView.BufferLocation	= this->resource->GetGPUVirtualAddress();
}

VertexBufferDx12::~VertexBufferDx12()
{
}

void VertexBufferDx12::setData(
	const void * data, 
	size_t size, 
	size_t offset)
{
	//void* dataToSet = &data + (size * offset);
	//Copy the triangle data to the vertex buffer.
	void* dataBegin = nullptr;
	//D3D12_RANGE range = { offset, offset+size}; 
	D3D12_RANGE range = { 0, 0 }; 
	this->resource->Map(0, &range, &dataBegin);
	memcpy(dataBegin, data, size);
	this->resource->Unmap(0, nullptr);

	//this->resourceView->StrideInBytes = size;
}

void VertexBufferDx12::bind(size_t offset, size_t size, unsigned int location)
{
	//rnd->commandList4->IASetVertexBuffers(0, 1, &resourceView);
	rnd->commandList4->IASetVertexBuffers(location, 1, &resourceView);
}

void VertexBufferDx12::unbind()
{
	rnd->commandList4->IASetVertexBuffers(0, 0, NULL);
}

size_t VertexBufferDx12::getSize()
{
	return this->totalSize;
}
