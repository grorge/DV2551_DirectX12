#include "Dx12Renderer.h"
#include "MaterialDx12.h"
#include "../Tools.h"

dxRenderer::dxRenderer()
{

}

dxRenderer::~dxRenderer()
{
}

Material * dxRenderer::makeMaterial(const std::string & name)
{

	//new MaterialDx12(name);

	return new MaterialDx12(name);
}

Mesh * dxRenderer::makeMesh()
{
	return nullptr;
}

VertexBuffer * dxRenderer::makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage)
{
	return nullptr;
}

ConstantBuffer * dxRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{


	return nullptr;
}

RenderState * dxRenderer::makeRenderState()
{
	return nullptr;
}

Technique * dxRenderer::makeTechnique(Material * m, RenderState * r)
{
	return nullptr;
}

Texture2D * dxRenderer::makeTexture2D()
{
	return nullptr;
}

Sampler2D * dxRenderer::makeSampler2D()
{
	return nullptr;
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

	window = SDL_CreateWindow("temp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	
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
	CreateDXGIFactory(IID_PPV_ARGS(&factory6));
	for (UINT adapterIndex = 0;; ++adapterIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory6->EnumAdapters1(adapterIndex, &adapter))
		{
			break; //No more adapters to enumerate.
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device5), nullptr)))
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

	// Here goes fence if we need it later



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
	device4->CreateDescriptorHeap(&heapDescriptorDesc, IID_PPV_ARGS(&descriptorHeap[CONST_DESC_HEAP_INDEX]));

	UINT constBuffersSize = device4->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh2 = descriptorHeap[CONST_DESC_HEAP_INDEX]->GetCPUDescriptorHandleForHeapStart();

	UINT cbSizeAligned = (sizeof(ConstantBuffer) + 255) & ~255;	// 256-byte aligned CB.

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

		cdh.ptr += constBuffersSize;
	}

	// ---------------- Root signature?

	return 0;
}

void dxRenderer::setWinTitle(const char * title)
{
}

int dxRenderer::shutdown()
{
	return 0;
}

void dxRenderer::setClearColor(float, float, float, float)
{
}

void dxRenderer::clearBuffer(unsigned int)
{
}

void dxRenderer::setRenderState(RenderState * ps)
{
}

void dxRenderer::submit(Mesh * mesh)
{
}

void dxRenderer::frame()
{
}

void dxRenderer::present()
{
}
