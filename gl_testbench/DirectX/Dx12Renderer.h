#pragma once

#include "../Renderer.h"
#include "RenderStateDx12.h"
#include "Texture2DDx12.h"
#include "Sampler2DDx12.h"
#include "VertexBufferDx12.h"
#include "MeshDx12.h"
#include "ConstantBufferDx12.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <GL/glew.h>

#include <windows.h>
#include "D3D12Header.h"

////#pragma comment(lib, "dire")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")


#define NUM_SWAP_BUFFERS 2
#define NUM_CONST_BUFFERS 16
#define NUM_SRV 4
#define NUM_SAMPLERS 4

#define NUM_DESCRIPTOR_HEAPS 2
#define CONST_DESC_HEAP_INDEX 0
#define SAMPLER_DESC_HEAP_INDEX 1

//struct ConstantBuffer
//{
//	float colorChannel[4];
//};

class dxRenderer : public Renderer
{
	friend class MaterialDx12;
	friend class Texture2DDx12;
	friend class Sampler2DDx12;
	friend class VertexBufferDx12;
	friend class ConstantBufferDx12;

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

	void setClearColor(float r, float g, float b, float a);
	void clearBuffer(unsigned int option);
	//	void setRenderTarget(RenderTarget* rt); // complete parameters
	void setRenderState(RenderState* ps);
	void submit(Mesh* mesh);
	void frame();
	void renderTest();
	void present();

	ID3D12Device4* GetDevice4() { return this->device4; }


	ID3D12Resource1*			vertexBufferResource;

private:
	SDL_Window* window;
	HWND hwnd;

	std::vector<Mesh*> drawList;
	std::vector<MeshDx12*> drawListDx12;
	std::unordered_map<Technique*, std::vector<Mesh*>> drawList2;

	bool globalWireframeMode = false;

	//int initializeOpenGL(int major, int minor, unsigned int width, unsigned int height);
	float clearColor[4] = { 0,0,0,0 };
	//std::unordered_map<int, int> BUFFER_MAP = {
	//	{0, 0},
	//	{CLEAR_BUFFER_FLAGS::COLOR, GL_COLOR_BUFFER_BIT },
	//	{CLEAR_BUFFER_FLAGS::DEPTH, GL_DEPTH_BUFFER_BIT },
	//	{CLEAR_BUFFER_FLAGS::STENCIL, GL_STENCIL_BUFFER_BIT }
	//};

	void CreateDirect3DDevice(HWND wndHandle);				//2. Create Device
	void CreateCommandInterfacesAndSwapChain(HWND wndHandle);	//3. Create CommandQueue and SwapChain
	void CreateFenceAndEventHandle();							//4. Create Fence and Event handle
	void CreateRenderTargets();									//5. Create render targets for backbuffer
	void CreateViewportAndScissorRect();						//6. Create viewport and rect
	void CreateShadersAndPiplelineState();						//7. Set up the pipeline state
	void CreateTriangleData();									//8. Create vertexdata
	void CreateRootSignature();
	void CreateConstantBufferResources();
	void wait4GPU();


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

	D3D12_VERTEX_BUFFER_VIEW	vertexBufferView;

	//Stefans cBuffers
	ID3D12DescriptorHeap*	descriptorHeap[NUM_SWAP_BUFFERS] = {};
	ID3D12DescriptorHeap*		descriptorHeapConstBuffers = {};
	ID3D12DescriptorHeap*		descriptorHeapSampler = {};
	ID3D12Resource1*			constantBufferResource[NUM_CONST_BUFFERS] = {};
	//ConstantBuffer			gConstantBufferCPU = {};

	ID3D12PipelineState*		pipelineState;

	VertexBuffer* vertBuffer;

	VertexBuffer* posRnd;
	VertexBuffer* norRnd;
	VertexBuffer* uvsRnd;

	int samplerCount;

	float colorBuffer[4] = { 0.0f, 1.0f, 1.0f, 1.0f };

};

