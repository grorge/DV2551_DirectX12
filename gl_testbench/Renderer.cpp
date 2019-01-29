#include "OpenGL/OpenGLRenderer.h"
#include "DirectX/Dx12Renderer.h"
#include "Renderer.h"


Renderer* Renderer::makeRenderer(BACKEND option)
{
	if (option == BACKEND::GL45)
		return new OpenGLRenderer();
	if (option == BACKEND::DX12)
		return new dxRenderer();
	else
		return nullptr;
}

