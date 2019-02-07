#pragma once
#include "VertexBufferDx12.h"


class MeshDx12
{
public:
	MeshDx12();
	MeshDx12(VertexBuffer* vertBuffer);
	~MeshDx12();

	void readyDraw();
private:
	VertexBuffer* vertBuffer;
};
