#pragma once

#include "../Texture2D.h"
#include "Sampler2DDx12.h"


class Texture2DDx12 :
	public Texture2D
{
public:
	Texture2DDx12();
	~Texture2DDx12();

	int loadFromFile(std::string filename);
	void bind(unsigned int slot);

	// OPENGL HANDLE
	//GLuint textureHandle = 0;
};

