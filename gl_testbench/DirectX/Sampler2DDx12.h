#pragma once
#include "../Sampler2D.h"
#include "D3D12Header.h"

class dxRenderer;

class Sampler2DDx12 :
	public Sampler2D
{
public:
	Sampler2DDx12();
	Sampler2DDx12(dxRenderer* rnd, int location);
	~Sampler2DDx12();

	void setMagFilter(FILTER filter);
	void setMinFilter(FILTER filter);
	void setWrap(WRAPPING s, WRAPPING t);

	//GLuint magFilter, minFilter, wrapS, wrapT;
	//GLuint samplerHandler = 0;
private:
	D3D12_SAMPLER_DESC samplerDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandler;
	dxRenderer *rnd;
};

