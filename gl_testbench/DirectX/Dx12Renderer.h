#pragma once

#include "../Renderer.h"

#include <SDL.h>
#include <SDL_syswm.h>

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h> //Only used for initialization of the device and swap chain.
#include <d3dcompiler.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")

//#pragma comment(lib, "dire")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")


#define NUM_SWAP_BUFFERS 3

//struct ConstantBuffer
//{
//	float colorChannel[4];
//};

class dxRenderer : public Renderer
{
public:
	dxRenderer();
	~dxRenderer();

	Material* makeMaterial(const std::string& name);
	Mesh* makeMesh();
	//VertexBuffer* makeVertexBuffer();
	VertexBuffer* makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage);
	ConstantBuffer* makeConstantBuffer(std::string NAME, unsigned int location);
	//	ResourceBinding* makeResourceBinding();
	RenderState* makeRenderState();
	Technique* makeTechnique(Material* m, RenderState* r);
	Texture2D* makeTexture2D();
	Sampler2D* makeSampler2D();
	std::string getShaderPath();
	std::string getShaderExtension();

	int initialize(unsigned int width = 640, unsigned int height = 480);
	void setWinTitle(const char* title);
	int shutdown();

	void setClearColor(float, float, float, float);
	void clearBuffer(unsigned int);
	//	void setRenderTarget(RenderTarget* rt); // complete parameters
	void setRenderState(RenderState* ps);
	void submit(Mesh* mesh);
	void frame();
	void present();

private:
	SDL_Window* window;
	HWND hwnd;

	std::vector<Mesh*> drawList;
	std::unordered_map<Technique*, std::vector<Mesh*>> drawList2;

	bool globalWireframeMode = false;

	//int initializeOpenGL(int major, int minor, unsigned int width, unsigned int height);
	float clearColor[4] = { 0,0,0,0 };
	std::unordered_map<int, int> BUFFER_MAP = {
		{0, 0},
		{CLEAR_BUFFER_FLAGS::COLOR, GL_COLOR_BUFFER_BIT },
		{CLEAR_BUFFER_FLAGS::DEPTH, GL_DEPTH_BUFFER_BIT },
		{CLEAR_BUFFER_FLAGS::STENCIL, GL_STENCIL_BUFFER_BIT }
	};

	void CreateDirect3DDevice(HWND wndHandle);				//2. Create Device
	void CreateCommandInterfacesAndSwapChain(HWND wndHandle);	//3. Create CommandQueue and SwapChain
	void CreateFenceAndEventHandle();							//4. Create Fence and Event handle
	void CreateRenderTargets();									//5. Create render targets for backbuffer
	void CreateViewportAndScissorRect();						//6. Create viewport and rect
	void CreateShadersAndPiplelineState();						//7. Set up the pipeline state
	void CreateTriangleData();									//8. Create vertexdata
	void CreateRootSignature();
	void CreateConstantBufferResources();


	ID3D12Device4*				device4 = nullptr;
	ID3D12GraphicsCommandList3*	commandList4 = nullptr;

	ID3D12CommandQueue*			commandQueue = nullptr;
	ID3D12CommandAllocator*		commandAllocator = nullptr;
	IDXGISwapChain4*			swapChain4 = nullptr;

	ID3D12Fence1*				fence = nullptr;
	HANDLE						eventHandle = nullptr;
	UINT64						fenceValue = 0;

	ID3D12DescriptorHeap*		renderTargetsHeap = nullptr;
	ID3D12Resource1*			renderTargets[NUM_SWAP_BUFFERS] = {};
	UINT						renderTargetDescriptorSize = 0;
	//UINT						frameIndex							= 0;

	D3D12_VIEWPORT				viewport = {};
	D3D12_RECT					scissorRect = {};

	ID3D12RootSignature*		rootSignature;
	ID3D12PipelineState*		pipeLineState;

	ID3D12Resource1*			vertexBufferResource;
	D3D12_VERTEX_BUFFER_VIEW	vertexBufferView;

	//Stefans cBuffers
	ID3D12DescriptorHeap*		descriptorHeap[NUM_SWAP_BUFFERS] = {};
	ID3D12Resource1*			constantBufferResource[NUM_SWAP_BUFFERS] = {};
	//ConstantBuffer			gConstantBufferCPU = {};

};

