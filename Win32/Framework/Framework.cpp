#include "stdafx.h"
#include "Scene\Title\TitleScene.h"
#include "Framework.h"
#include "Framework/Timer/Timer.h"

CDirectXFramework::CDirectXFramework()
{
}

CDirectXFramework::~CDirectXFramework()
{
	ReleaseObjects();
}

bool CDirectXFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	::GetClientRect(m_hWnd, &m_rcClient);

	CreateHwndRenderTarget();

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);
	::SetTimer(m_hWnd, 0, 16, reinterpret_cast<TIMERPROC>(CDirectXFramework::TimerProc));

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



void CDirectXFramework::Render()
{

	Render2D();

}

void CDirectXFramework::Update(float fTimeElapsed) const
{
	if (m_pCurrentScene) m_pCurrentScene->AnimateObjects(fTimeElapsed);
}



void CDirectXFramework::CreateHwndRenderTarget()
{
	const auto& IndRes = m_spIndRes.get();
	m_hWndRenderTarget = ::CreateHwndRenderTarget(IndRes->d2dFactory(), m_hWnd);
}



void CDirectXFramework::Render2D()
{
	m_hWndRenderTarget->BeginDraw();
	{

		m_hWndRenderTarget->Clear(ColorF{ ColorF::Gainsboro });
		if (m_pCurrentScene) m_pCurrentScene->Render2D(m_hWndRenderTarget.Get());

	}
	HRESULT hResult = m_hWndRenderTarget->EndDraw(); //	End Draw
	if (hResult == D2DERR_RECREATE_TARGET)
	{
		ReleaseDeviceDependentResources();
		CreateHwndRenderTarget();
	}
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
		break;
	}

	return(false);
}

LRESULT CDirectXFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// 선처리 구문 : 개별 Scene에서 처리할 것인지 확인
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			auto nWndClientWidth = static_cast<LONG>(LOWORD(lParam));
			auto nWndClientHeight = static_cast<LONG>(HIWORD(lParam));

			if (nWndClientWidth < CLIENT_MINIMUM_WIDTH - GetMarginWidth())
				nWndClientWidth = CLIENT_MINIMUM_WIDTH - GetMarginWidth();
				
			if (nWndClientHeight < CLIENT_MINIMUM_HEIGHT - GetMarginHeight())
				nWndClientHeight = CLIENT_MINIMUM_HEIGHT - GetMarginHeight();

			m_rcClient = RECT { 0, 0, nWndClientWidth, nWndClientHeight };

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

			Render();

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

	default:
		break;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

bool CDirectXFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F1:
			m_FullScreen.SetFullscreen_WindowMode(hWnd, m_rcClient);
			break;
		case VK_ESCAPE:
			PostMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		default: 
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}


void CDirectXFramework::FrameAdvance()
{
	// 지속적인 상수버퍼의 갱신을 확인하기 위한 Update 함수
	Update(static_cast<float>(m_timer->GetTimeElapsed()));

	Render();
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

void CDirectXFramework::TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	CDirectXFramework* self = ::GetUserDataPtr<CDirectXFramework*>(hWnd);
	self->FrameAdvance();
}

void CDirectXFramework::ChangeScene(wstring Tag, bool bDestroyPostScene)
{
FindScene:
	auto ChangeScene = find_if(m_lstScenes.begin(), m_lstScenes.end(),
		[&] (const unique_ptr<CScene>& s)
	{ return s->FindByTag(Tag); }
	);

	if (ChangeScene == m_lstScenes.end())
	{
	//	if (Tag == TEXT("Title"))
	//	{
	//		BuildTitleScene();
	//		goto FindScene;
	//	}
	//	else 
		return _DEBUG_ERROR("생성되지 않은 Scene을 참조하려 했습니다!");
	}

	if (m_pCurrentScene && bDestroyPostScene)
	{
		wstring DestroyTag = m_pCurrentScene->GetTag();

		m_pCurrentScene = nullptr;

		m_lstScenes.remove_if(
			[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(DestroyTag); }
		);
	}

	m_pCurrentScene = ChangeScene->get();
}

CScene * CDirectXFramework::FindScene(std::wstring Tag)
{
	auto ChangeScene = find_if(m_lstScenes.begin(), m_lstScenes.end(),
		[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(Tag); }
	);
	if (ChangeScene == m_lstScenes.end())
		return nullptr;
	return ChangeScene->get();
}
