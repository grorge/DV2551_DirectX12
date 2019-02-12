#include "MaterialDx12.h"
#include "Dx12Renderer.h"

#include <iostream>

void MaterialDx12::setShader(const std::string & shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		removeShader(type);
	}
	shaderFileNames[type] = shaderFileName;
}

void MaterialDx12::removeShader(ShaderType type)
{
}

int MaterialDx12::compileMaterial(std::string & errString)
{
	// remove all shaders.
	removeShader(ShaderType::VS);
	removeShader(ShaderType::PS);

	// compile shaders
	std::string err;
	if (compileShader(ShaderType::VS, err) < 0) {
		errString = err;
		exit(-1);
	};
	if (compileShader(ShaderType::PS, err) < 0) {
		errString = err;
		exit(-1);
	};

	//rnd->GetDevice4();
	// try to link the program
	// link shader program (connect vs and ps)
	//if (program != 0)
	//	glDeleteProgram(program);
	
	////// Input Layout //////
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		2, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = ARRAYSIZE(inputElementDesc);

	const D3D12_DEPTH_STENCILOP_DESC defStencilOP =
	{
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_KEEP,
		D3D12_COMPARISON_FUNC_ALWAYS
	};
	D3D12_DEPTH_STENCIL_DESC dsDesc = { };
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace = defStencilOP;
	dsDesc.BackFace = defStencilOP;

	////// Pipline State //////
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};

	//Specify pipeline stages:
	gpsd.pRootSignature = rnd->rootSignature;
	gpsd.InputLayout = inputLayoutDesc;
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.VS.pShaderBytecode = reinterpret_cast<void*>(vertexBlob->GetBufferPointer());
	gpsd.VS.BytecodeLength = vertexBlob->GetBufferSize();
	gpsd.PS.pShaderBytecode = reinterpret_cast<void*>(pixelBlob->GetBufferPointer());
	gpsd.PS.BytecodeLength = pixelBlob->GetBufferSize();

	//Specify render target and depthstencil usage.
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsd.NumRenderTargets = 1;

	gpsd.DepthStencilState = dsDesc;
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	gpsd.SampleDesc.Count = 1;
	gpsd.SampleMask = UINT_MAX;

	//Specify rasterizer behaviour.
	if (identification == 0)
		gpsd.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	else
		gpsd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	//Specify blend descriptions.
	D3D12_RENDER_TARGET_BLEND_DESC defaultRTdesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsd.BlendState.RenderTarget[i] = defaultRTdesc;

	HRESULT hr = rnd->device4->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&this->pipeLineState));

	isValid = true;
	return 0;
}

int MaterialDx12::enable()
{
	rnd->commandList4->SetPipelineState(this->pipeLineState);

	/*void* mappedMem = nullptr;
	this->rnd->constantBufferResource[CONST_BUFFER_COLOR]->Map(0, nullptr, &mappedMem);
	memcpy(mappedMem, buffer[0]->data(), buffer[0]->size());
	this->rnd->constantBufferResource[CONST_BUFFER_COLOR]->Unmap(0, nullptr);*/

	//rnd->commandList4->IASetVertexBuffers(0, 1, NULL);
	return 0;
}

void MaterialDx12::disable()
{
}

void MaterialDx12::setDiffuse(Color c)
{
}

void MaterialDx12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
	////Update GPU memory
	/*void* mappedMem = nullptr;

	this->rnd->constantBufferResource[location]->Map(0, nullptr, &mappedMem);
	
	memcpy(mappedMem, &data, size);

	D3D12_RANGE writeRange = { 0, size };
	this->rnd->constantBufferResource[location]->Unmap(0, &writeRange);*/

	this->buffer[0]->setData(data, size, this, 0);
}


void MaterialDx12::addConstantBuffer(std::string name, unsigned int location)
{
	/*std::wstring stemp = std::wstring(name.begin(), name.end());
	LPCWSTR sw = stemp.c_str();

	if (location >= NUM_CONST_BUFFERS) return; 

	rnd->constantBufferResource[location]->SetName(sw);*/

	this->buffer[0] = static_cast<ConstantBufferDx12*>
		(this->rnd->makeConstantBuffer(name, location));
}

int MaterialDx12::id()
{
	return this->identification;
}

void * MaterialDx12::constantBufferData(int location)
{
	return buffer[location]->data();
}

int MaterialDx12::compileShader(ShaderType type, std::string & errString)
{
	////// Shader Compiles //////
	HRESULT hr = S_FALSE;

	D3D_SHADER_MACRO* macro = nullptr;

	/*for (const auto &myPair : myMap) {
		std::cout << myPair.first << "\n";
	}*/
	std::string vs = "#define SHADER_ID " + std::to_string(identification) + "\n";
	std::string ps = "";

	std::string filepath = shaderFileNames[type];
	std::wstring wFilepath = std::wstring(filepath.begin(), filepath.end());
	LPCWSTR lpcwFilePath = wFilepath.c_str();

	for (auto element : this->shaderDefines) {
		for (auto s : element.second)
		{
			vs.append(s);
			ps.append(s);
		}
	}

	vs.append("struct VSIn { float4 pos : POSITION0; float4 nor : NORMAL0; float2 uv : TEXCOORD0; }; struct VSOut { float4 pos : SV_POSITION; float4 nor : NORMAL0; float4 color : COLOR0; float2 uv : TEXCOORD0; }; cbuffer descriptorHeapConstantBuffer1 : register (b0) { float4 translate[512]; } cbuffer descriptorHeapConstantBuffer2 : register (b1) { float4 color[512]; } VSOut main( float4 position : POSITION0, float4 normal : NORMAL0, float2 uv : TEXCOORD0, uint vertex : SV_VertexID, uint instance : SV_InstanceID) { VSOut output = (VSOut)0; output.pos = position + translate[instance + (SHADER_ID * 25)]; output.nor = normal; output.uv = uv; output.color = color[instance  + (SHADER_ID * 25)]; return output; }");
	ps.append("struct VSOut { float4 pos : SV_POSITION; float4 nor : NORMAL0; float4 color : COLOR0; float2 uv : TEXCOORD0; }; float4 main(VSOut input) : SV_TARGET0 { /*return float4(1.0f, 0.0f, 1.0f, 1.0f);*/ return input.color; }");

	std::cout << vs << std::endl;


	switch (type)
	{
	case Material::ShaderType::VS:

		/*unsigned int len = shaderDefines.size();

		macro = new D3D_SHADER_MACRO[len];

		for (int i = 0; i < len; i++)
		{
			
			macro[i].Name = shaderDefines.at(Material::ShaderType::VS, this->name);
			macro[i].Definition = shaderDefines.;
		}*/

		hr = D3DCompile(
			vs.c_str(),
			vs.length(),
			NULL,
			NULL,
			NULL, //D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&this->vertexBlob,
			nullptr
		);

		//hr = D3DCompileFromFile(
		//	lpcwFilePath, // filename
		//	macro,		// optional macros
		//	nullptr,		// optional include files
		//	"main",		// entry point
		//	"vs_5_0",		// shader model (target)
		//	0,				// shader compile options			// here DEBUGGING OPTIONS
		//	0,				// effect compile options
		//	&this->vertexBlob,	// double pointer to ID3DBlob		
		//	nullptr			// pointer for Error Blob messages.
		//					// how to use the Error blob, see here
		//					// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		//);
		break;

	case Material::ShaderType::PS:

		hr = D3DCompile(
			ps.c_str(),
			ps.length(),
			NULL,
			NULL,
			NULL, //D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&this->pixelBlob,
			nullptr
		);

		//hr = D3DCompileFromFile(
		//	lpcwFilePath,				// filename
		//	nullptr,		// optional macros
		//	nullptr,		// optional include files
		//	"main",		// entry point
		//	"ps_5_0",		// shader model (target)
		//	0,				// shader compile options			// here DEBUGGING OPTIONS
		//	0,				// effect compile options
		//	&this->pixelBlob,		// double pointer to ID3DBlob		
		//	nullptr			// pointer for Error Blob messages.
		//					// how to use the Error blob, see here
		//					// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		//);
		break;
	case Material::ShaderType::GS:
		break;
	case Material::ShaderType::CS:
		break;
	default:
		break;
	}

	return hr == S_OK ? 1 : -1;
}

std::vector<std::string> MaterialDx12::expandShaderText(std::string & shaderText, ShaderType type)
{
	return std::vector<std::string>();
}

MaterialDx12::MaterialDx12(const std::string & name)
{
	this->name = name;

	this->vertexBlob = nullptr;
	this->pixelBlob = nullptr;
}

MaterialDx12::MaterialDx12(const std::string & name, dxRenderer* rnd, int id)
{
	this->name = name;

	this->rnd = rnd;

	this->vertexBlob = nullptr;
	this->pixelBlob = nullptr;

	this->identification = id;
}

MaterialDx12::~MaterialDx12()
{
}
