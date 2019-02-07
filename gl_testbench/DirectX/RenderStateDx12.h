#pragma once
#include <vector>
#include "../RenderState.h"

class RenderStateDx12 : public RenderState
{
public:
	RenderStateDx12();
	~RenderStateDx12();
	void setWireFrame(bool);
	void set();

	void setGlobalWireFrame(bool* global);
private:
	bool _wireframe;
	bool* globalWireFrame;
};

