#include "stdafx.h"
#include "Framework/IndRes/independentdeviceresources.h"
#include "Resource/resource.h"
#include "header/GetThemeData.h"

#include "CustomCaptionUI.h"



CCustomCaptionUI::~CCustomCaptionUI()
{
}

void CCustomCaptionUI::Draw(ID2D1RenderTarget * pd2dRenderTarget)
{
	
	
	pd2dRenderTarget->FillRectangle(GetRect(UI_PROPERTY::Client), m_pd2dsbrBackground.Get());
	pd2dRenderTarget->DrawBitmap(m_pd2dbmpIcon.Get(), m_rcIcon);
	pd2dRenderTarget->DrawTextLayout(
		  Point2F(28, 5)
		, m_tlCaption.Get()
		, m_pd2dsbrCaptionText.Get()
		, D2D1_DRAW_TEXT_OPTIONS_CLIP
	);
}

void CCustomCaptionUI::OnPrepareRender(ID2D1RenderTarget * pd2dRenderTarget)
{
	auto sz = GetRelatedStartPosition();
	auto rc = GetRelatedDrawableRect();
	pd2dRenderTarget->GetTransform(&m_mtxStoreLast);
	pd2dRenderTarget->SetTransform(Matrix3x2F::Translation(sz));
	pd2dRenderTarget->PushAxisAlignedClip(rc, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void CCustomCaptionUI::OnFinishedRender(ID2D1RenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->PopAxisAlignedClip();
	pd2dRenderTarget->SetTransform(m_mtxStoreLast);
}

void CCustomCaptionUI::Update(float fTimeElapsed)
{
}

void CCustomCaptionUI::Build(std::string Tag, const shared_ptr<CIndRes>& indres, ID2D1RenderTarget * pd2dRenderTarget)
{
	auto color = ~GetCaptionColor();
	ColorF captionColor{ ColorF::Black, 1.f };
	captionColor.r = static_cast<float>(GetRValue(color)) / 255.f;
	captionColor.g = static_cast<float>(GetGValue(color)) / 255.f;
	captionColor.b = static_cast<float>(GetBValue(color)) / 255.f;

	color = ~GetCaptionTextColor();
	ColorF captionTextColor{ ColorF::Black, 1.f };
	captionTextColor.r = static_cast<float>(GetRValue(color)) / 255.f;
	captionTextColor.g = static_cast<float>(GetGValue(color)) / 255.f;
	captionTextColor.b = static_cast<float>(GetBValue(color)) / 255.f;
	pd2dRenderTarget->CreateSolidColorBrush(captionColor, &m_pd2dsbrBackground);
	pd2dRenderTarget->CreateSolidColorBrush(captionTextColor, &m_pd2dsbrCaptionText);

	

	// Load Icon
	m_pd2dbmpIcon = LoadFromIcon(
		  indres->wicFactory.Get()
		, pd2dRenderTarget
		, GetHInstance()
		, MAKEINTRESOURCE(IDI_WIN32)
	);
	
	m_fIconSize = GetSystemMetrics(SM_CYSMICON);
	m_rcIcon = RectF(0, 0, m_fIconSize, m_fIconSize) + Point2F(8, 6);

	indres->dwFactory->CreateTextFormat(
		  GetCaptionFontName().c_str()
		, NULL
		, DWRITE_FONT_WEIGHT_NORMAL
		, DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH_NORMAL
		, 12
		, L"ko-kr"
		, &m_tfCaption
	);

	indres->dwFactory->CreateTextLayout(
		  TITLESTRING
		, lstrlen(TITLESTRING)
		, m_tfCaption.Get()
		, CLIENT_WIDTH
		, CaptionHeight
		, &m_tlCaption
	);

}

bool CCustomCaptionUI::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		return false;
	}
	return(true);
}

bool CCustomCaptionUI::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		DefWindowProc(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		break;

	case WM_RBUTTONDOWN:
		DefWindowProc(hWnd, WM_NCRBUTTONDOWN, HTCAPTION, lParam);
		break;

	case WM_RBUTTONUP:
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		if (!hMenu) break;

		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		mii.fType = 0;

		// update the options
		mii.fState = MF_ENABLED;
		SetMenuItemInfo(hMenu, SC_RESTORE, FALSE, &mii);
		SetMenuItemInfo(hMenu, SC_SIZE, FALSE, &mii);
		SetMenuItemInfo(hMenu, SC_MOVE, FALSE, &mii);
		SetMenuItemInfo(hMenu, SC_MAXIMIZE, FALSE, &mii);
		SetMenuItemInfo(hMenu, SC_MINIMIZE, FALSE, &mii);

		mii.fState = MF_GRAYED;

		WINDOWPLACEMENT wp;
		GetWindowPlacement(hWnd, &wp);

		switch (wp.showCmd)
		{
		case SW_SHOWMAXIMIZED:
			SetMenuItemInfo(hMenu, SC_SIZE, FALSE, &mii);
			SetMenuItemInfo(hMenu, SC_MOVE, FALSE, &mii);
			SetMenuItemInfo(hMenu, SC_MAXIMIZE, FALSE, &mii);
			SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
			break;
		case SW_SHOWMINIMIZED:
			SetMenuItemInfo(hMenu, SC_MINIMIZE, FALSE, &mii);
			SetMenuDefaultItem(hMenu, SC_RESTORE, FALSE);
			break;
		case SW_SHOWNORMAL:
			SetMenuItemInfo(hMenu, SC_RESTORE, FALSE, &mii);
			SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
			break;
		}

		DWORD style = GetWindowStyle(hWnd);

		if (style & WS_BORDER)
		{
			SetMenuItemInfo(hMenu, SC_RESTORE, FALSE, &mii);
			SetMenuItemInfo(hMenu, SC_SIZE, FALSE, &mii);
			SetMenuItemInfo(hMenu, SC_MAXIMIZE, FALSE, &mii);
		}

		// hard
		SetMenuItemInfo(hMenu, SC_MOVE, FALSE, &mii);

		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);
		
		auto cmd = TrackPopupMenu(
			  hMenu
			, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD
			, GET_X_LPARAM(lParam) + rcWindow.left
			, GET_Y_LPARAM(lParam) + rcWindow.top
			, 0
			, hWnd
			, NULL
		); 

		if (cmd) PostMessage(hWnd, WM_SYSCOMMAND, cmd, 0);

		DestroyMenu(hMenu);
	}
	break;

//	case WM_MOUSEMOVE:
//		break;
//
//	case WM_LBUTTONUP:
//		break;

	default:
		return false;
	}

	return true;
}

bool CCustomCaptionUI::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;

	default:
		return false;
	}

	return true;
}


