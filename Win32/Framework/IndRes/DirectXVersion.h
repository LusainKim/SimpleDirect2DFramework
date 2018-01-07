#pragma once

#define USING_D3D11_INDRES	0
#define USING_D2D1_INDRES	1



#if USING_D3D11_INDRES

#include <d3d11_2.h>
#include <dxgi1_3.h>



using D3D_Device		= ID3D11Device2;
using D3D_DeviceContext	= ID3D11DeviceContext2;

#endif



#if USING_D2D1_INDRES

#include <d2d1_2.h>
#include <dwrite_2.h>
#include <d2d1_2helper.h>
#include <wincodec.h>



using D2D_Factory = ID2D1Factory2;
using DWT_Factory = IDWriteFactory2;
using WIC_Factory = IWICImagingFactory2;

#endif

#if USING_D3D11_INDRES && USING_D2D1_INDRES

class CIndependentDeviceResource;
using CIndRes = CIndependentDeviceResource;

#elif USING_D3D11_INDRES 

class CDirect3DIndependentDeviceResource;
using CIndRes = CDirect3DIndependentDeviceResource;

#elif USING_D2D1_INDRES

class CDirect2DIndependentDeviceResource;
using CIndRes = CDirect2DIndependentDeviceResource;

#endif



