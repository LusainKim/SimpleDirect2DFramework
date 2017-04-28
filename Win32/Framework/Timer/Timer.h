#pragma once



class CTimer
{

public:
	CTimer();
	virtual ~CTimer();

	bool Update();
	double GetFPS() const { return { 0.5 + m_olddCumulativefps / static_cast<double>(m_oldnCumulativefpsCount) }; }
	double GetTimeElapsed() const { return m_TimeElapsed; }

	void SetUpdateCaptionHwnd(HWND hWnd);
	void UpdateCaption();

protected:

	std::chrono::system_clock::time_point m_current_time;

	float		m_TimeElapsed = 0.f;

	float		m_dCumulativefps = 0.0;
	int			m_nCumulativefpsCount = 0;

	float		m_olddCumulativefps = 0.0;
	int			m_oldnCumulativefpsCount = 0;

	std::chrono::system_clock::time_point m_LastUpdate_time;


	HWND		m_hWnd	{nullptr };
	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH];
	int			m_TitleLength{ 0 };

#if defined(SHOW_CAPTIONFPS)
	int			m_nLastFps{ 0 };
#endif

};