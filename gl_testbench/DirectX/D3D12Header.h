#pragma once
// lissajous points
typedef union {
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
} float4;

typedef union {
	struct { float x, y; };
	struct { float u, v; };
} float2;


#include <d3d12.h>
#pragma comment (lib, "d3d12.lib")

#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")

//Only used for initialization of the device and swap chain.
#include <dxgi1_6.h> 
#pragma comment (lib, "DXGI.lib")