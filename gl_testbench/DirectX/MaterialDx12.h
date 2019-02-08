#pragma once

#include "../Material.h"
#include <vector>
#include <Windows.h>
#include "D3D12Header.h"
#include "ConstantBufferDx12.h"



class dxRenderer;

//#define DBOUTW( s )\
//{\
//std::wostringstream os_;\
//os_ << s;\
//OutputDebugStringW( os_.str().c_str() );\
//}
//
//#define DBOUT( s )\
//{\
//std::ostringstream os_;\
//os_ << s;\
//OutputDebugString( os_.str().c_str() );\
//}
//
//// use X = {Program or Shader}
//#define INFO_OUT(S,X) { \
//char buff[1024];\
//memset(buff, 0, 1024);\
//glGet##X##InfoLog(S, 1024, nullptr, buff);\
//DBOUTW(buff);\
//}
//
//// use X = {Program or Shader}
//#define COMPILE_LOG(S,X,OUT) { \
//char buff[1024];\
//memset(buff, 0, 1024);\
//glGet##X##InfoLog(S, 1024, nullptr, buff);\
//OUT=std::string(buff);\
//}

#define NUM_CONST_BUFFER 1

class MaterialDx12 :
	public Material
{
	friend dxRenderer;

public:
	MaterialDx12(const std::string& name);
	MaterialDx12(const std::string& name, dxRenderer* rnd, int id);
	~MaterialDx12();


	void setShader(const std::string& shaderFileName, ShaderType type);
	void removeShader(ShaderType type);
	int compileMaterial(std::string& errString);
	int enable();
	void disable();
	UINT getProgram() { return program; };
	void setDiffuse(Color c);

	// location identifies the constant buffer in a unique way
	void updateConstantBuffer(const void* data, size_t size, unsigned int location);
	// slower version using a string
	void addConstantBuffer(std::string name, unsigned int location);
	//std::map<unsigned int, ConstantBufferGL*> constantBuffers;

	int id();
	void* constantBufferData(int location);

private:
	// map from ShaderType to GL_VERTEX_SHADER, should be static.
	UINT mapShaderEnum[4];

	std::string shaderNames[4];

	// opengl shader object
	UINT shaderObjects[4] = { 0,0,0,0 };

	// TODO: change to PIPELINE
	// opengl program object
	std::string name;
	UINT program;
	int compileShader(ShaderType type, std::string& errString);
	std::vector<std::string> expandShaderText(std::string& shaderText, ShaderType type);


	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;

	ID3D12PipelineState* pipeLineState;

	ConstantBufferDx12* buffer[NUM_CONST_BUFFER];
	dxRenderer* rnd = nullptr;
	int identification;
};
