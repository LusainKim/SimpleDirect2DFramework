#include "stdafx.h"
#include "Framework\Framework.h"
#include "TitleScene.h"



CTitleScene::CTitleScene()
{
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CTitleScene::BuildObjects(std::wstring Tag, HWND hWnd, CDirectXFramework * pMasterFramework)
{
	CScene::BuildObjects(Tag, hWnd, pMasterFramework);
}

void CTitleScene::ReleaseObjects()
{
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
	Update2D(fTimeElapsed);
}

void CTitleScene::BuildObjecsFromD2D1Devices(ID2D1Factory * pd2dDevice, ID2D1HwndRenderTarget * pd2dDeviceContext, IDWriteFactory2 * pdwFactory, IWICImagingFactory2 * pwicFactory)
{
}


void CTitleScene::Update2D(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
}

void CTitleScene::Render2D(ID2D1HwndRenderTarget * pd2dDeviceContext)
{
	ComPtr<ID2D1SolidColorBrush> hbr;
	pd2dDeviceContext->CreateSolidColorBrush(ColorF{ ColorF::AliceBlue }, &hbr);

	float angle = XMConvertToRadians(m_fTick * 270.f);
	float x = 50 + cos(angle) * 25.f;
	float y = 50 + sin(angle) * 25.f;
	pd2dDeviceContext->FillRectangle(RectF(x - 10.f, y - 10.f, x + 10.f, y + 10.f), hbr.Get());
}
