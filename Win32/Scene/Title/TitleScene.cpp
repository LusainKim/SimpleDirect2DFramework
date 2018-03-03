#include "stdafx.h"
#include "Framework/Framework.h"
#include "Framework/Input/Input.h"

#include "UI/ColorUI/ColorUI.h"

#include "TitleScene.h"



CTitleScene::CTitleScene()
	: m_uiManager { this }
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

void CTitleScene::Build(std::string Tag, CDirectXFramework * pMasterFramework)
{
	CScene::Build(Tag, pMasterFramework);
	
	auto pColorUI = make_unique<CColorUI>(&m_uiManager, m_SelectColor);
	pColorUI->InitializeStartPosition(Point2F(0, 200));
	pColorUI->InitializeClinet(RectF(0, 0, 256, 256));
	pColorUI->Build("Color"s, m_pIndRes, pMasterFramework->GetRenderTarget());

	m_uiManager.Insert(move(pColorUI));

	BuildHSVPicker();
}

void CTitleScene::BuildHSVPicker()
{

	Color pixels[360][1];
	for (int i = 0; i < 360; ++i)
		pixels[360 - (1 + i)][0] = Color::HSVtoRGB(Color{ static_cast<float>(i), 1, 1 });

	auto rc = RectU(0, 0, 1, 360);
	auto rendertarget = m_pMasterFramework->GetRenderTarget();
	ComPtr<ID2D1Bitmap> bmp;
	rendertarget->CreateBitmap(SizeU(rc.right, rc.bottom)
		, BitmapProperties(PixelFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, D2D1_ALPHA_MODE_PREMULTIPLIED))
		, &bmp);

	bmp.As(&m_pd2dbmpColorsPallet);

	m_pd2dbmpColorsPallet->CopyFromMemory(&rc, &pixels, 1 * sizeof(Color));
}

void CTitleScene::ReleaseObjects()
{
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
	m_uiManager.Update(fTimeElapsed);
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

	pd2dDeviceContext->DrawBitmap(m_pd2dbmpColorsPallet.Get(), RectF(260, 200, 270, 456));

	m_uiManager.Draw(pd2dDeviceContext);
}

void CTitleScene::BindKey()
{
	using namespace InputManager;

	make_button(move_left);
	move_left.down_event;
	m_InputManager->bind(VK_LEFT, move(move_left));
}
