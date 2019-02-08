#include "MeshDx12.h"

MeshDx12::MeshDx12()
{
	//this->geometryBuffers[0].buffer;
	//rnd->commandList4->IASetVertexBuffers(location, 1, &this->resourceView);
}

MeshDx12::MeshDx12(VertexBuffer * vertBuffer)
{
	this->vertBuffer = vertBuffer;
}


MeshDx12::~MeshDx12()
{
}

void MeshDx12::readyDraw()
{
	this->vertBuffer->bind(0, 0, 0);
}
