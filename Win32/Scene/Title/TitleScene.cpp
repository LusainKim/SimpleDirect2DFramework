#include "stdafx.h"
#include "Framework/Framework.h"
#include "Framework/Input/Input.h"
#include "TitleScene.h"



CTitleScene::CTitleScene()
{
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

#include <windowsx.h>
bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	static bool bCapture = false;
	switch(nMessageID)
	{
	case WM_LBUTTONDOWN:
		bCapture = true;
	case WM_MOUSEMOVE:
		if (!bCapture)
			break;
	case WM_LBUTTONUP:
		m_Hvalue = max(min(256, 456 - HIWORD(lParam)), 0);
		m_Hvalue = m_Hvalue / 256.f * 360.f;
		if (m_Hvalue >= 360.f) m_Hvalue -= 360.f;
		BuildColorPicker();
		break;
	}
	if (nMessageID == WM_LBUTTONUP) bCapture = false;
	return true;
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch(nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		return OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		return OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	}
	return true;
}

struct Color 
{ 
	union
	{
		// rgb : [0, 1]
		struct { float r, g, b; };
		// h : [0, 360], sv : [0, 1]
		struct { float h, s, v; };
	};
	// alpha value
	float a;

	Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f) noexcept
		: r { r }
		, g { g }
		, b { b }
		, a { a }
	{
	}
};

Color HSVtoRGB(Color hsv)
{
	Color rgb;

	// achromatic (grey)
	if (hsv.s == 0.f) return Color{ hsv.v, hsv.v, hsv.v };

	// sector 0 to 5
	hsv.h /= 60.f;

	auto i = static_cast<int>(std::floor(hsv.h));

	// factorial part of h
	auto f = hsv.h - i;          
	auto p = hsv.v * (1.f - hsv.s);
	auto q = hsv.v * (1.f - hsv.s * f);
	auto t = hsv.v * (1.f - hsv.s * (1.f - f));

	switch (i) 
	{
	case 0:		return Color{ hsv.v, t, p };
	case 1:		return Color{ q, hsv.v, p };
	case 2:		return Color{ p, hsv.v, t };
	case 3:		return Color{ p, q, hsv.v };
	case 4:		return Color{ t, p, hsv.v };
	case 5:		return Color{ hsv.v, p, q };
	default:	return Color{};
	}
}

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
Color RGBtoHSV(Color rgb)
{
	Color hsv;

	float _min = std::min(std::min(rgb.r, rgb.g), rgb.b);
	float _max = std::max(std::max(rgb.r, rgb.g), rgb.b);
	float delta = _max - _min;

	// between yellow & magenta
	if (rgb.r == _max)			hsv.h = 0.f + (rgb.g - rgb.b) / delta;
	// between cyan & yellow
	else if (rgb.g == _max)		hsv.h = 2.f + (rgb.b - rgb.r) / delta;
	// between magenta & cyan
	else						hsv.h = 4.f + (rgb.r - rgb.g) / delta;	
	
	hsv.h *= 60.f;				// degrees
	
	if (hsv.h < 0) hsv.h += 360;

	// s
	if (_max != 0) hsv.s = delta / _max;		
	else 
	{
		// r = g = b = 0		// s = 0, v is undefined
		hsv.s = 0;
		hsv.h = -1.f;
		return hsv;
	}
	
	// v
	hsv.v = _max;	

	return hsv;
}

void CTitleScene::Build(std::string Tag, CDirectXFramework * pMasterFramework)
{
	CScene::Build(Tag, pMasterFramework);
	BuildColorPicker();
	BuildHSVPicker();
}

void CTitleScene::BuildHSVPicker()
{

	Color pixels[360][1];
	for (int i = 0; i < 360; ++i)
		pixels[360 - (1 + i)][0] = HSVtoRGB(Color{ static_cast<float>(i), 1, 1 });

	auto rc = RectU(0, 0, 1, 360);
	auto rendertarget = m_pMasterFramework->GetRenderTarget();
	ComPtr<ID2D1Bitmap> bmp;
	rendertarget->CreateBitmap(SizeU(rc.right, rc.bottom)
		, BitmapProperties(PixelFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, D2D1_ALPHA_MODE_PREMULTIPLIED))
		, &bmp);

	bmp.As(&m_pd2dbmpColorsPallet);

	m_pd2dbmpColorsPallet->CopyFromMemory(&rc, &pixels, 1 * sizeof(Color));
}

void CTitleScene::BuildColorPicker()
{

	constexpr size_t len = 64;
	Color pixels[len][len];

	Color hsv{ m_Hvalue };

	// v val per step. [0.0, 1.0)
	float val = 0.f;
	float step = 1.f / static_cast<float>(len - 1);

	for (int y = len - 1; y >= 0; --y)
	{
		// s val per step. (0.0, 1.0]
		hsv.s = 0.f;

		for (int x = 0; x < len; ++x)
		{
			pixels[y][x] = HSVtoRGB(hsv);
			hsv.s += step;
		}

		hsv.v += step;
	}

	//	wstring dbgstr =	to_wstring(pixels[0][0].r) + L", "s
	//					+	to_wstring(pixels[0][0].g) + L", "s
	//					+	to_wstring(pixels[0][0].b) + L"\n"s;
	//	OutputDebugString(dbgstr.c_str());
	//	
	//	dbgstr =	to_wstring(pixels[63][0].r) + L", "s
	//			+	to_wstring(pixels[63][0].g) + L", "s
	//			+	to_wstring(pixels[63][0].b) + L"\n"s;
	//	OutputDebugString(dbgstr.c_str());
	//	
	//	dbgstr =	to_wstring(pixels[0][63].r) + L", "s
	//			+	to_wstring(pixels[0][63].g) + L", "s
	//			+	to_wstring(pixels[0][63].b) + L"\n"s;
	//	OutputDebugString(dbgstr.c_str());
	//	
	//	dbgstr =	to_wstring(pixels[63][63].r) + L", "s
	//			+	to_wstring(pixels[63][63].g) + L", "s
	//			+	to_wstring(pixels[63][63].b) + L"\n"s;
	//	OutputDebugString(dbgstr.c_str());

	auto rc = RectU(0, 0, 64, 64);
	auto rendertarget = m_pMasterFramework->GetRenderTarget();
	ComPtr<ID2D1Bitmap> bmp;
	rendertarget->CreateBitmap(SizeU(rc.right, rc.bottom), BitmapProperties(PixelFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, D2D1_ALPHA_MODE_PREMULTIPLIED)), &bmp);

	bmp.As(&m_pd2dbmpColors);

	m_pd2dbmpColors->CopyFromMemory(&rc, &pixels, len * sizeof(Color));
}

void CTitleScene::ReleaseObjects()
{
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
}

void CTitleScene::Draw(ID2D1HwndRenderTarget * pd2dDeviceContext)
{
	ComPtr<ID2D1SolidColorBrush> hbr;
	pd2dDeviceContext->CreateSolidColorBrush(ColorF{ ColorF::AliceBlue }, &hbr);

	float angle = XMConvertToRadians(m_fTick * 270.f);
	float x = 50 + cos(angle) * 25.f;
	float y = 50 + sin(angle) * 25.f;
//	pd2dDeviceContext->FillRectangle(RectF(x - 10.f, y - 10.f, x + 10.f, y + 10.f), hbr.Get());
	ComPtr<ID2D1LinearGradientBrush> brush;
	
	ComPtr<ID2D1GradientStopCollection> pGradientStops;
	D2D1_GRADIENT_STOP gradientStops[2];
	gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
	gradientStops[0].position = 0.0f;
	gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1);
	gradientStops[1].position = 1.0f;
	pd2dDeviceContext->CreateGradientStopCollection(
		gradientStops,
		2,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_WRAP,
		&pGradientStops
	);

	pd2dDeviceContext->CreateLinearGradientBrush(
			LinearGradientBrushProperties(
				Point2F(0, 0),
				Point2F(150, 150)
			)
		,	pGradientStops.Get()
		,	&brush
	);

	pd2dDeviceContext->FillRectangle(RectF(0, 0, 150, 150), brush.Get());

	pd2dDeviceContext->DrawBitmap(m_pd2dbmpColors.Get(), RectF(0, 200, 256, 456));
	pd2dDeviceContext->DrawBitmap(m_pd2dbmpColorsPallet.Get(), RectF(260, 200, 270, 456));

	pd2dDeviceContext->DrawBitmap(m_pd2dbmpColors.Get(), RectF(500, 200, 564, 264));
	pd2dDeviceContext->DrawBitmap(m_pd2dbmpColorsPallet.Get(), RectF(570, 200, 580, 264));
}

void CTitleScene::BindKey()
{
	using namespace InputManager;

	make_button(move_left);
	move_left.down_event;
	m_InputManager->bind(VK_LEFT, move(move_left));
}
