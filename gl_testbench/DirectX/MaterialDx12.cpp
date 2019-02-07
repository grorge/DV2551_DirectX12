#include "MaterialDx12.h"
#include "Dx12Renderer.h"

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

	gpsd.SampleDesc.Count = 1;
	gpsd.SampleMask = UINT_MAX;

	//Specify rasterizer behaviour.
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

	rnd->device4->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&this->pipeLineState));

	//program = glCreateProgram();
	//glAttachShader(program, shaderObjects[(GLuint)ShaderType::VS]);
	//glAttachShader(program, shaderObjects[(GLuint)ShaderType::PS]);
	//glLinkProgram(program);

	//std::string err2;
	//INFO_OUT(program, Program);
	//COMPILE_LOG(program, Program, err2);
	isValid = true;
	return 0;
}

int MaterialDx12::enable()
{
	rnd->commandList4->SetPipelineState(this->pipeLineState);
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
	//Update GPU memory
	void* mappedMem = nullptr;
	D3D12_RANGE readRange = { 0, 0 }; //We do not intend to read this resource on the CPU.
	if (SUCCEEDED(rnd->constantBufferResource[location]->Map(0, &readRange, &mappedMem)))
	{
		memcpy(mappedMem, &data, size);

		D3D12_RANGE writeRange = { 0, size };
		rnd->constantBufferResource[location]->Unmap(0, &writeRange);
	}
}


void MaterialDx12::addConstantBuffer(std::string name, unsigned int location)
{
	std::wstring stemp = std::wstring(name.begin(), name.end());
	LPCWSTR sw = stemp.c_str();

	if (location >= NUM_CONST_BUFFERS) return; 

	rnd->constantBufferResource[location]->SetName(sw);
}

int MaterialDx12::compileShader(ShaderType type, std::string & errString)
{
	////// Shader Compiles //////
	HRESULT hr = S_FALSE;

	D3D_SHADER_MACRO* macro = nullptr;

	/*for (const auto &myPair : myMap) {
		std::cout << myPair.first << "\n";
	}*/

	std::string s = shaderFileNames[type];
	std::wstring stemp = std::wstring(s.begin(), s.end());
	LPCWSTR sw = stemp.c_str();

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

		hr = D3DCompileFromFile(
			sw, // filename
			macro,		// optional macros
			nullptr,		// optional include files
			"main",		// entry point
			"vs_5_0",		// shader model (target)
			0,				// shader compile options			// here DEBUGGING OPTIONS
			0,				// effect compile options
			&this->vertexBlob,	// double pointer to ID3DBlob		
			nullptr			// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);
		break;
	case Material::ShaderType::PS:
		hr = D3DCompileFromFile(
			sw,				// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			"main",		// entry point
			"ps_5_0",		// shader model (target)
			0,				// shader compile options			// here DEBUGGING OPTIONS
			0,				// effect compile options
			&this->pixelBlob,		// double pointer to ID3DBlob		
			nullptr			// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);
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
MaterialDx12::MaterialDx12(const std::string & name, dxRenderer* rnd)
{
	this->name = name;

	this->rnd = rnd;

	this->vertexBlob = nullptr;
	this->pixelBlob = nullptr;
}

MaterialDx12::~MaterialDx12()
{
}
