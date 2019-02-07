#pragma once

#include "../Texture2D.h"
#include "Sampler2DDx12.h"

#include "D3D12Header.h"

class dxRenderer;

class Texture2DDx12 :
	public Texture2D
{
public:
	Texture2DDx12();
	Texture2DDx12(dxRenderer* rnd);
	~Texture2DDx12();

	int loadFromFile(std::string filename);
	void bind(unsigned int slot);

	// OPENGL HANDLE
	//GLuint textureHandle = 0;

	dxRenderer* rnd;
	ID3D12Resource* resource;
};

