#include "stdafx.h"
#include "Framework/Framework.h"
#include "Framework/Input/Input.h"

#include "UI/ColorUI/ColorUI.h"
#include "UI/System/CustomCaptionUI.h"

#include "TitleScene.h"



CTitleScene::CTitleScene()
	: m_uiManager { this }
{
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

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
		{
			int startHeight = m_uiManager.find("Color"s)->GetRelatedStartPosition().height;
			int palletHeight = m_uiManager.find("Color"s)->GetRect(UI_PROPERTY::Client).bottom;
			m_SelectColor->h = std::max(std::min(palletHeight, ((startHeight + palletHeight) - GET_Y_LPARAM(lParam))), 0);
			m_SelectColor->h = m_SelectColor->h / static_cast<float>(palletHeight) * 360.f;
			if (m_SelectColor->h >= 360.f) m_SelectColor->h -= 360.f;
			m_SelectColor.SetNotify();
		}
		break;
	}
	if (nMessageID == WM_LBUTTONUP) bCapture = false;
	return true;
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_uiManager.OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam)) return true;

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
	pColorUI->InitializeStartPosition(Point2F(15, CaptionHeight + 15));
	pColorUI->InitializeClinet(RectF(0, 0, 180, 180));
	pColorUI->Build("Color"s, m_pIndRes, pMasterFramework->GetRenderTarget());

	m_uiManager.Insert(move(pColorUI));

	auto pCaptionUI = make_unique<CCustomCaptionUI>(&m_uiManager);
	pCaptionUI->InitializeStartPosition(Point2F(0, 0));
	pCaptionUI->InitializeClinet(RectF(0, 0, m_pMasterFramework->GetClientRect().right, CaptionHeight));
	pCaptionUI->Build("Caption"s, m_pIndRes, pMasterFramework->GetRenderTarget());

	m_uiManager.Insert(move(pCaptionUI));

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
	pd2dDeviceContext->DrawBitmap(m_pd2dbmpColorsPallet.Get()
		, RectF(220, CaptionHeight + 15, 230, CaptionHeight + 195));

	m_uiManager.Draw(pd2dDeviceContext);
}

void CTitleScene::BindKey()
{
	using namespace InputManager;

	make_button(move_left);
	move_left.down_event;
	m_InputManager->bind(VK_LEFT, move(move_left));
}
