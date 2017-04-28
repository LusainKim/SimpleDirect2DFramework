#include "stdafx.h"
#include "Timer.h"

CTimer::CTimer()
{
	// 타이머 초기화
	m_LastUpdate_time = chrono::system_clock::now();
	m_current_time = chrono::system_clock::now();

	m_dCumulativefps = 0.f;
	m_nCumulativefpsCount = 0;
}

CTimer::~CTimer()
{
}

bool CTimer::Update()
{
	std::chrono::duration<float> timeElapsed = chrono::system_clock::now() - m_current_time;

	float fps = 0.f;
	
	if (timeElapsed.count() > MAX_FPS)
	{
		m_current_time = chrono::system_clock::now();

		if (timeElapsed.count() > 0.0f)
		{
			m_TimeElapsed = timeElapsed.count();
			fps = 1.f / m_TimeElapsed;
		}
	}
	else return false;



	m_dCumulativefps += fps;
	m_nCumulativefpsCount++;

	std::chrono::duration<float> UpdateElapsed = chrono::system_clock::now() - m_LastUpdate_time;

	if (UpdateElapsed.count() > MAX_UPDATE_FPS)
		m_LastUpdate_time = chrono::system_clock::now();
	else return true;

	m_olddCumulativefps = m_dCumulativefps;
	m_oldnCumulativefpsCount = m_nCumulativefpsCount;

	m_dCumulativefps = 0.0;
	m_nCumulativefpsCount = 0;
//
//	m_dCumulativefps += fps;
//	m_nCumulativefpsCount++;

	if (m_hWnd) UpdateCaption();

	return true;
}

void CTimer::SetUpdateCaptionHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
	// 캡션 변경
#if defined(TITLESTRING)
	lstrcpy(m_CaptionTitle, TITLESTRING);
#else
	GetWindowText(m_hWnd, m_CaptionTitle, TITLE_MAX_LENGTH);
#endif
#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

}

void CTimer::UpdateCaption()
{
	// 프레임레이트를 출력합니다.
#if defined(SHOW_CAPTIONFPS)

#if USE_DEBUG_WINDOW
	Event_CutsomLogger eventShowFPS(string("FPS"), to_string(m_fps));
	CLogSystem::PropagateNotification(nullptr, &eventShowFPS);
#endif

	int nCurrentFps = static_cast<int>(GetFPS());

	if (m_nLastFps == nCurrentFps) return;

	m_nLastFps = nCurrentFps;

	_itow_s(m_nLastFps, m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif
}
