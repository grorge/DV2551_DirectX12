#include "Dx12Renderer.h"
#include "MaterialDx12.h"
#include "ConstantBufferDx12.h"
#include "../Tools.h"
#include "MeshDx12.h"

dxRenderer::dxRenderer()
{
	this->samplerCount = 0;
	this->materialCount = 0;
}

dxRenderer::~dxRenderer()
{
}

Material * dxRenderer::makeMaterial(const std::string & name)
{
	//new MaterialDx12(name);

	return new MaterialDx12(name, this, this->materialCount++);
}

Mesh * dxRenderer::makeMesh()
{
	return new MeshDx12();
}

VertexBuffer * dxRenderer::makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage)
{
	return new VertexBufferDx12(size, usage, this);
}

ConstantBuffer * dxRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
	return new ConstantBufferDx12(NAME, location);
}

RenderState * dxRenderer::makeRenderState()
{
	return new RenderStateDx12;
}

Technique * dxRenderer::makeTechnique(Material * m, RenderState * r)
{
	return new Technique(m,r);
}

Texture2D * dxRenderer::makeTexture2D()
{
	return new Texture2DDx12(this);
}

Sampler2D * dxRenderer::makeSampler2D()
{
	if (this->samplerCount >= NUM_SAMPLERS) return nullptr;

	return new Sampler2DDx12(this, this->samplerCount++);
}

std::string dxRenderer::getShaderPath()
{
	return std::string("..\\assets\\Dx12\\");
}

std::string dxRenderer::getShaderExtension()
{
	return std::string(".hlsl");
}

int dxRenderer::initialize(unsigned int width, unsigned int height)
{
	// --------------------Make window

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
		exit(-1);
	}

	window = SDL_CreateWindow("Dx12", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	hwnd = wmInfo.info.win.window;


	// ------------------Create device
#ifdef _DEBUG
	ID3D12Debug* debugController = nullptr;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	SafeRelease(&debugController);
#endif

	//dxgi1_6 is only needed for the initialization process using the adapter.
	IDXGIFactory6*	factory6 = nullptr;
	IDXGIAdapter1*	adapter = nullptr;
	//First a factory is created to iterate through the adapters available.
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory6));
	for (UINT adapterIndex = 0;; ++adapterIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory6->EnumAdapters1(adapterIndex, &adapter))
		{
			break; //No more adapters to enumerate.
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device4), nullptr)))
		{
			break;
		}

		SafeRelease(&adapter);
	}
	if (adapter)
	{
		//Create the actual device.
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device4));
	
		SafeRelease(&adapter);
	}
	else
	{
		return 1;
	}

	SafeRelease(&factory6);

	// -----------------------Interface and SwCh

	//Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC cqd = {};
	device4->CreateCommandQueue(&cqd, IID_PPV_ARGS(&commandQueue));

	//Create command allocator. The command allocator object corresponds
	//to the underlying allocations in which GPU commands are stored.
	device4->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

	//Create command list.
	device4->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator,
		nullptr,
		IID_PPV_ARGS(&commandList4));

	//Command lists are created in the recording state. Since there is nothing to
	//record right now and the main loop expects it to be closed, we close it.
	commandList4->Close();

	IDXGIFactory5*	factory5 = nullptr;
	CreateDXGIFactory(IID_PPV_ARGS(&factory5));

	//Create swap chain.
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = 0;
	scDesc.Height = 0;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = FALSE;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = NUM_SWAP_BUFFERS;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Flags = 0;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	IDXGISwapChain1* swapChain1 = nullptr;
	if (SUCCEEDED(factory5->CreateSwapChainForHwnd(
		commandQueue,
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		&swapChain1)))
	{
		if (SUCCEEDED(swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain4))))
		{
			//swapChain4->Release();
		}
	}

	SafeRelease(&factory5);
	SafeRelease(&swapChain1);

	// -------------------- Fence

	this->device4->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	this->fenceValue = 1;
	//Create an event handle to use for GPU synchronization.
	this->eventHandle = CreateEvent(0, false, false, 0);

	// -------------------- Rendertarget

	//Create descriptor heap for render target views.
	D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
	dhd.NumDescriptors = NUM_SWAP_BUFFERS;
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	device4->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&renderTargetsHeap));

	//Create resources for the render targets.
	renderTargetDescriptorSize = device4->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetsHeap->GetCPUDescriptorHandleForHeapStart();

	//One RTV for each frame.
	for (UINT n = 0; n < NUM_SWAP_BUFFERS; n++)
	{
		swapChain4->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n]));
		device4->CreateRenderTargetView(renderTargets[n], nullptr, cdh);
		cdh.ptr += renderTargetDescriptorSize;
	}


	// -------------- Viewport and ScissorRect
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = (long)0;
	scissorRect.right = (long)width;
	scissorRect.top = (long)0;
	scissorRect.bottom = (long)height;


	// ---------------- Constant buffer Rescources

	D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorDesc = {};
	heapDescriptorDesc.NumDescriptors = NUM_CONST_BUFFERS;
	heapDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device4->CreateDescriptorHeap(&heapDescriptorDesc, IID_PPV_ARGS(&descriptorHeapConstBuffers));

	UINT constBuffersSize = device4->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh2 = descriptorHeapConstBuffers->GetCPUDescriptorHandleForHeapStart();

	UINT align = 32 * 64;
	UINT cbSizeAligned = align;// (sizeof(/*TOTAL_TRIS*/100 * 4 * sizeof(float)) + align) & ~align;	// 256-byte aligned CB.

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.CreationNodeMask = 1; //used when multi-gpu
	heapProperties.VisibleNodeMask = 1; //used when multi-gpu
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = cbSizeAligned;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Create a resource heap, descriptor heap, and pointer to cbv for each frame
	for (int i = 0; i < NUM_CONST_BUFFERS; i++)
	{
		HRESULT hr = device4->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBufferResource[i])
		);

		constantBufferResource[i]->SetName(L"cb heap");

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = constantBufferResource[i]->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = cbSizeAligned;
		device4->CreateConstantBufferView(&cbvDesc, cdh2);

		cdh2.ptr += constBuffersSize;
	}

	// ---------------- Create Sampler

	D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorDescSampler = {};
	heapDescriptorDescSampler.NumDescriptors = NUM_SAMPLERS;
	heapDescriptorDescSampler.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDescriptorDescSampler.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	device4->CreateDescriptorHeap(&heapDescriptorDescSampler, IID_PPV_ARGS(&descriptorHeapSampler));

	/*
	UINT SamplerSize = device4->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh3 = descriptorHeap[SAMPLER_DESC_HEAP_INDEX]->GetCPUDescriptorHandleForHeapStart();
	*/

	// --------------------- Depth Stencil

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = { };
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = this->device4->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&this->dsDescriptorHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsStencilViewDesc = { };
	dsStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptClearValue = { };
	depthOptClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptClearValue.DepthStencil.Depth = 1.0f;
	depthOptClearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES dsHeapProp = {};
	dsHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	dsHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	dsHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	dsHeapProp.CreationNodeMask = 0;
	dsHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC dsResourceDesc = { };
	dsResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsResourceDesc.Alignment = 65536; //Wah?
	dsResourceDesc.Width = this->viewport.Width;
	dsResourceDesc.Height = this->viewport.Height;
	dsResourceDesc.DepthOrArraySize = 1;
	dsResourceDesc.MipLevels = 1;
	dsResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsResourceDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };
	dsResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	hr = this->device4->CreateCommittedResource(
		&dsHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&dsResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptClearValue,
		IID_PPV_ARGS(&this->depthStencilBuffer)
	);

	this->dsDescriptorHeap->SetName(L"DepthStencil Resource Heap");

	this->device4->CreateDepthStencilView(this->depthStencilBuffer, &dsStencilViewDesc, this->dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	// ---------------- Root signature

	//define descriptor range(s)
	D3D12_DESCRIPTOR_RANGE  dtRangesCBV[2];
	dtRangesCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dtRangesCBV[0].NumDescriptors = 1;
	dtRangesCBV[0].BaseShaderRegister = 0; //register b0
	dtRangesCBV[0].RegisterSpace = 0; //register(b0,space0);
	dtRangesCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	dtRangesCBV[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dtRangesCBV[1].NumDescriptors = 1;
	dtRangesCBV[1].BaseShaderRegister = 1; //register b1
	dtRangesCBV[1].RegisterSpace = 0; //register(b1,space0);
	dtRangesCBV[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE dtCBV;
	dtCBV.NumDescriptorRanges = ARRAYSIZE(dtRangesCBV);
	dtCBV.pDescriptorRanges = dtRangesCBV;


	//define descriptor range(s)
	D3D12_DESCRIPTOR_RANGE  dtRangesSRV[1];
	dtRangesSRV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dtRangesSRV[0].NumDescriptors = NUM_SRV; 
	dtRangesSRV[0].BaseShaderRegister = 0; //register t0
	dtRangesSRV[0].RegisterSpace = 0; //register(t0,space1);
	dtRangesSRV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE dtSRV;
	dtSRV.NumDescriptorRanges = ARRAYSIZE(dtRangesSRV);
	dtSRV.pDescriptorRanges = dtRangesSRV;




	//create root parameter
	D3D12_ROOT_PARAMETER  rootParam[2];
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[0].DescriptorTable = dtCBV;
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[1].DescriptorTable = dtSRV;
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumParameters = ARRAYSIZE(rootParam);
	rsDesc.pParameters = rootParam;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.pStaticSamplers = nullptr;

	ID3DBlob* sBlob;
	D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sBlob,
		nullptr);

	device4->CreateRootSignature(
		0,
		sBlob->GetBufferPointer(),
		sBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));


	// --------------------- 

	


	return 0;
}

void dxRenderer::setWinTitle(const char * title)
{
	SDL_SetWindowTitle(this->window, title);
}

int dxRenderer::shutdown()
{
	return 0;
}

void dxRenderer::setClearColor(float r, float g, float b, float a)
{
	this->clearColor[0] = r;
	this->clearColor[1] = g;
	this->clearColor[2] = b;
	this->clearColor[3] = a;
}

void SetResourceTransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
	D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = StateBefore;
	barrierDesc.Transition.StateAfter = StateAfter;

	commandList->ResourceBarrier(1, &barrierDesc);
}

void dxRenderer::clearBuffer(unsigned int option)
{

	//Command list allocators can only be reset when the associated command lists have
	//finished execution on the GPU; fences are used to ensure this (See WaitForGpu method)
	commandAllocator->Reset();
	commandList4->Reset(commandAllocator, NULL);


	//Indicate that the back buffer will be used as render target.
	SetResourceTransitionBarrier(
		this->commandList4,
		this->renderTargets[this->swapChain4->GetCurrentBackBufferIndex()],
		D3D12_RESOURCE_STATE_PRESENT,		//state before
		D3D12_RESOURCE_STATE_RENDER_TARGET	//state after
	);

	D3D12_CPU_DESCRIPTOR_HANDLE cdh = this->renderTargetsHeap->GetCPUDescriptorHandleForHeapStart();
	cdh.ptr += this->renderTargetDescriptorSize * this->swapChain4->GetCurrentBackBufferIndex();

	this->commandList4->ClearRenderTargetView(cdh, clearColor, 0, nullptr);
	this->commandList4->ClearDepthStencilView(this->dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	if (3 == option)
	{
	}
}

void dxRenderer::setRenderState(RenderState * ps)
{

}

void dxRenderer::submit(Mesh * mesh)
{
	//drawList.push_back(mesh);

	switch (static_cast<MaterialDx12*>(mesh->technique->getMaterial())->id())
	{
	case 0:
		dl0.push_back(mesh);
		break;

	case 1:
		dl1.push_back(mesh);
		break;

	case 2:
		dl2.push_back(mesh);
		break;

	case 3:
		dl3.push_back(mesh);
		break;

	default:
		break;
	}
}

void dxRenderer::frame()
{

	UINT backBufferIndex = this->swapChain4->GetCurrentBackBufferIndex();

	//Set constant buffer descriptor heap
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapConstBuffers };
	//ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap[backBufferIndex] };
	this->commandList4->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);

	this->commandList4->SetGraphicsRootSignature(this->rootSignature);
	
	//Set root descriptor table to index 0 in previously set root signature
	this->commandList4->SetGraphicsRootDescriptorTable(
		0,
		descriptorHeapConstBuffers->GetGPUDescriptorHandleForHeapStart());

	//Set necessary states.
	this->commandList4->RSSetViewports(1, &this->viewport);
	this->commandList4->RSSetScissorRects(1, &this->scissorRect);

	

	D3D12_CPU_DESCRIPTOR_HANDLE cdh = this->renderTargetsHeap->GetCPUDescriptorHandleForHeapStart();
	cdh.ptr += this->renderTargetDescriptorSize * backBufferIndex;

	this->commandList4->OMSetRenderTargets(1, &cdh, true, 
		&this->dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);
	this->commandList4->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	size_t count = dl0.size() + dl1.size() + dl2.size() + dl3.size();
	UINT byteWidth = sizeof(float) * 4;
	void* translationData	= malloc(byteWidth * count);
	void* colorData			= malloc(byteWidth * count);

	void* mappedMem = nullptr;
	D3D12_RANGE writeRange = { 0, byteWidth * count };

	int i = 0;
	UINT offset = 0;
	for (Mesh* mesh : dl0)
	{
		offset = i * byteWidth;
		mesh->technique->enable(this);

		ConstantBufferDx12 *translationBuffer = 
			static_cast<ConstantBufferDx12*>(mesh->txBuffer);

		memcpy(static_cast<char*>(translationData) + offset,
			translationBuffer->data(), byteWidth);

		MaterialDx12 *material =
			static_cast<MaterialDx12*>(mesh->technique->getMaterial());

		memcpy(static_cast<char*>(colorData) + offset,
			material->constantBufferData(0), byteWidth);

		for (auto element : mesh->geometryBuffers) {
			mesh->bindIAVertexBuffer(element.first);
		}

		i++;
	}

	for (Mesh* mesh : dl1)
	{
		offset = i * byteWidth;
		mesh->technique->enable(this);

		ConstantBufferDx12 *translationBuffer =
			static_cast<ConstantBufferDx12*>(mesh->txBuffer);

		memcpy(static_cast<char*>(translationData) + offset,
			translationBuffer->data(), byteWidth);

		MaterialDx12 *material =
			static_cast<MaterialDx12*>(mesh->technique->getMaterial());

		memcpy(static_cast<char*>(colorData) + offset,
			material->constantBufferData(0), byteWidth);

		for (auto element : mesh->geometryBuffers) {
			mesh->bindIAVertexBuffer(element.first);
		}

		i++;
	}

	for (Mesh* mesh : dl2)
	{
		offset = i * byteWidth;
		mesh->technique->enable(this);

		ConstantBufferDx12 *translationBuffer =
			static_cast<ConstantBufferDx12*>(mesh->txBuffer);

		memcpy(static_cast<char*>(translationData) + offset,
			translationBuffer->data(), byteWidth);

		MaterialDx12 *material =
			static_cast<MaterialDx12*>(mesh->technique->getMaterial());

		memcpy(static_cast<char*>(colorData) + offset,
			material->constantBufferData(0), byteWidth);

		for (auto element : mesh->geometryBuffers) {
			mesh->bindIAVertexBuffer(element.first);
		}

		i++;
	}

	for (Mesh* mesh : dl3)
	{
		offset = i * byteWidth;
		mesh->technique->enable(this);

		ConstantBufferDx12 *translationBuffer =
			static_cast<ConstantBufferDx12*>(mesh->txBuffer);

		memcpy(static_cast<char*>(translationData) + offset,
			translationBuffer->data(), byteWidth);

		MaterialDx12 *material =
			static_cast<MaterialDx12*>(mesh->technique->getMaterial());

		memcpy(static_cast<char*>(colorData) + offset,
			material->constantBufferData(0), byteWidth);

		for (auto element : mesh->geometryBuffers) {
			mesh->bindIAVertexBuffer(element.first);
		}

		i++;
	}

	mappedMem = nullptr;
	writeRange = { 0, byteWidth * count };
	this->constantBufferResource[CONST_BUFFER_TRANSLATION]->Map(0, nullptr, &mappedMem);
	memcpy(mappedMem, translationData, byteWidth * count);
	this->constantBufferResource[CONST_BUFFER_TRANSLATION]->Unmap(0, &writeRange);

	this->constantBufferResource[CONST_BUFFER_COLOR]->Map(0, nullptr, &mappedMem);
	memcpy(mappedMem, colorData, byteWidth * count);
	this->constantBufferResource[CONST_BUFFER_COLOR]->Unmap(0, &writeRange);

	this->dl0.front()->technique->enable(this);
	this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);
	this->dl1.front()->technique->enable(this);
	this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);
	this->dl2.front()->technique->enable(this);
	this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);
	this->dl3.front()->technique->enable(this);
	this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);

	//Update GPU memory
	/*void* mappedMem = nullptr;
	D3D12_RANGE writeRange = { 0, byteWidth * count };
	this->constantBufferResource[CONST_BUFFER_TRANSLATION]->Map(0, nullptr, &mappedMem);
	memcpy(mappedMem, translationData, byteWidth * count);
	this->constantBufferResource[CONST_BUFFER_TRANSLATION]->Unmap(0, &writeRange);

	this->constantBufferResource[CONST_BUFFER_COLOR]->Map(0, nullptr, &mappedMem);
	memcpy(mappedMem, colorData, byteWidth * count);
	this->constantBufferResource[CONST_BUFFER_COLOR]->Unmap(0, &writeRange);

	this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);*/
	//this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);
	//this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);
	//this->commandList4->DrawInstanced(3, (UINT)25, 0, 0);

	//Indicate that the back buffer will be used as render target.
	SetResourceTransitionBarrier(
		this->commandList4,
		this->renderTargets[backBufferIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,	//flipped
		D3D12_RESOURCE_STATE_PRESENT		
	);

	this->commandList4->Close();

	ID3D12CommandList* listsToExecute[] = { commandList4 };
	this->commandQueue->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);

	free(translationData);
}

void dxRenderer::present()
{
	//Present the frame.
	DXGI_PRESENT_PARAMETERS pp = {};
	this->swapChain4->Present1(0, 0, &pp);
	this->dl0.clear();
	this->dl1.clear();
	this->dl2.clear();
	this->dl3.clear();

	this->wait4GPU();	
}

void dxRenderer::wait4GPU()
{
	//Signal and increment the fence value.
	const UINT64 fence = fenceValue;
	commandQueue->Signal(this->fence, fence);
	fenceValue++;

	//Wait until command queue is done.
	if (this->fence->GetCompletedValue() < fence)
	{
		this->fence->SetEventOnCompletion(fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
	}
}
