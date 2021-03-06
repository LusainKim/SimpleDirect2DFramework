#include "stdafx.h"
#include "Scene/Title/TitleScene.h"
#include "Framework.h"
#include "Framework/Timer/Timer.h"

CDirectXFramework::CDirectXFramework()
{
}

CDirectXFramework::~CDirectXFramework()
{
	m_InputManager.SaveAllKey("Main", "./config.ini");
	ReleaseObjects();
}

bool CDirectXFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	::GetClientRect(m_hWnd, &m_rcClient);

	backgroundColor = ColorF { ColorF::Black, 0.75f };
	BindKey();

	CreateHwndRenderTarget();

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	return true;
}

void CDirectXFramework::ReleaseObjects()
{
	ReleaseDeviceDependentResources();
}

void CDirectXFramework::ReleaseDeviceDependentResources()
{
	m_hWndRenderTarget = nullptr;
}



void CDirectXFramework::Update(float fTimeElapsed) const
{
	if (m_pCurrentScene) m_pCurrentScene->AnimateObjects(fTimeElapsed);
}



void CDirectXFramework::CreateHwndRenderTarget()
{
	const auto& IndRes = m_spIndRes.get();
	m_hWndRenderTarget = ::CreateHwndRenderTarget(
		  IndRes->d2dFactory.Get()
		, m_hWnd
		, RenderTargetProperties(
			  D2D1_RENDER_TARGET_TYPE_DEFAULT
			, PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		  )
	);
}



void CDirectXFramework::Draw()
{
	m_hWndRenderTarget->BeginDraw();
	
	m_hWndRenderTarget->Clear(backgroundColor);
	if (m_pCurrentScene)
		m_pCurrentScene->Draw(m_hWndRenderTarget.Get());
	
	if (m_hWndRenderTarget->EndDraw() != D2DERR_RECREATE_TARGET) 
		return;

	ReleaseDeviceDependentResources();
	CreateHwndRenderTarget();
}



bool CDirectXFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		return false;
	}

	return true;
}

LRESULT CDirectXFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// 선처리 구문 : 개별 Scene에서 처리할 것인지 확인
	switch (nMessageID)
	{
	case WM_SIZE:
		{
		
			auto nWndClientWidth  = GET_X_LPARAM(lParam);
			auto nWndClientHeight = GET_Y_LPARAM(lParam);

			if (nWndClientWidth < CLIENT_MINIMUM_WIDTH - GetMarginWidth())
				nWndClientWidth = CLIENT_MINIMUM_WIDTH - GetMarginWidth();
				
			if (nWndClientHeight < CLIENT_MINIMUM_HEIGHT - GetMarginHeight())
				nWndClientHeight = CLIENT_MINIMUM_HEIGHT - GetMarginHeight();

			::SetRect(&m_rcClient, 0, 0, nWndClientWidth, nWndClientHeight);

			m_hWndRenderTarget->Resize(SizeU(nWndClientWidth, nWndClientHeight));
			
		//	ReleaseDeviceDependentResources();
		//	auto IndRes = IndependentResources();
		//	CreateHwndRenderTarget();
		//	if (m_pCurrentScene)
		//		m_pCurrentScene->BuildObjecsFromD2D1Devices
		//		(
		//			  IndRes->d2dFactory()
		//			, GetRenderTarget()
		//			, IndRes->dwFactory()
		//			, IndRes->wicFactory()
		//		);

			Draw();

			return DefWindowProc(hWnd, nMessageID, wParam, lParam);
		}

	default:
		if (m_pCurrentScene)	if (m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam)) break;
								else return 0;
	}

	// 후처리 구문 : Scene에서 처리되지 않고 남는 부분을 처리
	switch (nMessageID)
	{
	case WM_GETMINMAXINFO:

	//	((MINMAXINFO*) lParam)->ptMaxTrackSize.x = 300;
	//	((MINMAXINFO*) lParam)->ptMaxTrackSize.y = 200;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = CLIENT_MINIMUM_WIDTH;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = CLIENT_MINIMUM_HEIGHT;

		return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	case WM_ACTIVATE:
		{
			auto ncPolicy = DWMNCRP_DISABLED;
			DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &ncPolicy, sizeof(ncPolicy));
		}		

	case WM_DWMCOMPOSITIONCHANGED:
		{
			// https://msdn.microsoft.com/ko-kr/library/aa969512.aspx
			MARGINS margins{ -1 };
			DwmExtendFrameIntoClientArea(hWnd, &margins);
		}
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

void CDirectXFramework::BindKey()
{
	make_button(exit);
	exit.up_event = [&]() { DestroyWindow(m_hWnd); };

	make_button(fullscreen);
	fullscreen.up_event = [&]() { m_FullScreen.SetFullscreen_WindowMode(m_hWnd, m_rcClient); };

	m_InputManager.bind(VK_ESCAPE, move(exit), "Main", "./config.ini");
	m_InputManager.bind(VK_F1, move(fullscreen), "Main", "./config.ini");

}

bool CDirectXFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
//		switch (wParam)
//		{
//		default: 
//			return true;
//		}
//		break;
	default:
		return true;
	}
//	return(false);
}

void CDirectXFramework::ProcessInput()
{
	bool arrByte[256]{};
	memset(arrByte, false, sizeof BYTE * 256);

	if (m_pCurrentScene) m_pCurrentScene->ProcessInput(arrByte);

	m_InputManager.ProcessInput(arrByte);
}

void CDirectXFramework::FrameAdvance()
{
	ProcessInput();
	// 지속적인 상수버퍼의 갱신을 확인하기 위한 Update 함수
	Update(static_cast<float>(m_timer->GetTimeElapsed()));

	Draw();
}



LRESULT CALLBACK CDirectXFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CDirectXFramework* self = ::GetUserDataPtr<CDirectXFramework*>(hWnd);
	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	auto DestroyWindow = [&] ()
	{
		bool bQuitable = self->IsQuitable();
		::SetUserDataPtr(hWnd, nullptr);
		if (bQuitable)
			::PostQuitMessage(0);
	};
	
	switch (nMessageID)
	{
//	case WM_CREATE:
//		break;
//
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		::BeginPaint(hWnd, &ps);
//		::EndPaint(hWnd, &ps);
//	}
//	break;

	case WM_DESTROY:
		DestroyWindow();
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}

void CDirectXFramework::ChangeScene(const string& Tag, bool bDestroyPostScene)
{
	auto ChangeScene = find_if(begin(m_lstScenes), end(m_lstScenes),
		[&] (const unique_ptr<CScene>& s)
	{ return s->FindByTag(Tag); }
	);

	if (end(m_lstScenes) == ChangeScene)
	{
		::MessageBox(m_hWnd, L"생성되지 않은 Scene을 참조하려 했습니다!", L"find Failed!", MB_OK);
		::DestroyWindow(m_hWnd);
		return;
	}

	if (m_pCurrentScene)
	{
		// leave old scene
		m_pCurrentScene->LeaveScene();

		// destroy scene
		if (bDestroyPostScene)
		{
			auto&& DestroyTag = m_pCurrentScene->tag;

			m_pCurrentScene = nullptr;

			m_lstScenes.remove_if(
				[&] (const unique_ptr<CScene>& s)
			{ return s->FindByTag(DestroyTag); }
			);
		}
	}

	// change current scene
	m_pCurrentScene = ChangeScene->get();
	// enter new scene
	m_pCurrentScene->EnterScene();
}

CScene * CDirectXFramework::FindScene(const std::string& Tag)
{
	auto ChangeScene = find_if(begin(m_lstScenes), end(m_lstScenes),
		[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(Tag); }
	);
	if (end(m_lstScenes) == ChangeScene)
		return nullptr;
	return ChangeScene->get();
}
