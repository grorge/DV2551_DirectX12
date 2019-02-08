#include "RenderStateDx12.h"

RenderStateDx12::RenderStateDx12()
{
	_wireframe = false;
	globalWireFrame = nullptr; 
}

RenderStateDx12::~RenderStateDx12()
{
}

void RenderStateDx12::setWireFrame(bool input)
{
	this->_wireframe = input;
}

void RenderStateDx12::set()
{
}

void RenderStateDx12::setGlobalWireFrame(bool * global)
{
	this->globalWireFrame = global;
}
