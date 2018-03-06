#pragma once

#include <wrl/client.h>
#include <d2d1.h>
#include <wincodec.h>

using Microsoft::WRL::ComPtr;


static ComPtr<ID2D1Bitmap> LoadFromIcon( 
				  IWICImagingFactory* wicFactory
				, ID2D1RenderTarget* pd2dRenderTarget
				, HINSTANCE hInstance
				, LPCWSTR lpIconName
				, REFWICPixelFormatGUID dstFormat = GUID_WICPixelFormat32bppPBGRA)
{
	ComPtr<ID2D1Bitmap>	bmp;
	ComPtr<IWICBitmap>	wicBmp;

	HICON hIcon = LoadIcon(hInstance, lpIconName);
	if (!hIcon) return bmp;

	if (FAILED(wicFactory->CreateBitmapFromHICON(hIcon, &wicBmp))) 
		return bmp;

	ComPtr<IWICFormatConverter> wicFormatConverter;
	wicFactory->CreateFormatConverter(&wicFormatConverter);
	if (FAILED(wicFormatConverter->Initialize(
		  wicBmp.Get()
		, dstFormat
		, WICBitmapDitherTypeNone
		, nullptr
		, 0.f
		, WICBitmapPaletteTypeMedianCut
	))) 
		return bmp;

	pd2dRenderTarget->CreateBitmapFromWicBitmap(wicFormatConverter.Get(), &bmp);
	DeleteObject(hIcon);

	return bmp;
}