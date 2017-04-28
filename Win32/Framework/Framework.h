#pragma once

class CScene;

class CTimer;

#include "header/MakeFullscreen.h"
#include "IndRes/IndependentDeviceResources.h"
using CIndRes = CDirect2DIndependentDeviceResource;

///	<remarks> 
///	Create 함수는 객체의 할당 여부를 검사하지 않습니다. 할당될 객체들은 모두 해제되는 등 할당되지 않은 상태여야 합니다.
/// 모든 할당되지 않은 객체는 nullptr를 유지해야 합니다.
///	</remarks>
class CDirectXFramework {

private:

	HINSTANCE						m_hInstance					{ nullptr }	;
	HWND							m_hWnd						{ nullptr }	;
	RECT							m_rcClient;

	CFullScreen						m_FullScreen;

	shared_ptr<CIndRes>				m_spIndRes;

public:

	CDirectXFramework();

	virtual ~CDirectXFramework();

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void ReleaseObjects();

	void ReleaseDeviceDependentResources();



	void Render();

	void Update(float fTimeElapsed) const;



	void InjectIndependentResources(shared_ptr<CIndRes> res) { m_spIndRes = res; }
	void EquipTimer(shared_ptr<CTimer> timer) { m_timer = timer; }

	// Getter Method입니다.
public:

	HWND GetHwnd()			const { return m_hWnd; }

	RECT GetClientRect() const { return m_rcClient; }
	D2D_RECT_F   GetClientRectF() const { return RectF(1.f * m_rcClient.left, 1.f * m_rcClient.top, 1.f * m_rcClient.right, 1.f * m_rcClient.bottom); }
	D2D_POINT_2F GetCenterPos()   const { return Point2F(m_rcClient.right * 0.5f, m_rcClient.bottom * 0.5f); }
	D2D_SIZE_U   GetClientSize()  const { return SizeU(static_cast<UINT32>(m_rcClient.right), static_cast<UINT32>(m_rcClient.bottom)); }

	auto IndependentResources()	const { return m_spIndRes.get(); }

	ID2D1HwndRenderTarget* GetRenderTarget() const { return m_hWndRenderTarget.Get(); }

	// 2D code
private:

	ComPtr<ID2D1HwndRenderTarget>		m_hWndRenderTarget	{ nullptr }	;


public:
	
	void CreateHwndRenderTarget();

	void Render2D();



// 게임 전체의 로직을 담당합니다.
private:

	std::list<std::unique_ptr<CScene>>		m_lstScenes;
	CScene								*	m_pCurrentScene	{ nullptr }	;

	// Network 
	shared_ptr<CTimer>						m_timer;

	bool									m_bMainWindow { false };

public:

	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//	Scene을 만드는 함수를 인자로 받는 함수입	니다. 사용법은 다음과 같습니다.
	//	BuildScene([&](CDirectXFramework *framework, 추가 인자...) -> unique_ptr<CScene> { } );
	template<typename Fn, typename... Args>
	void BuildScene(Fn&& CreateBuildedScene, Args... args)
	{
		auto pScene = std::forward<Fn>(CreateBuildedScene)(this, std::forward<Args>(args)...);
		m_lstScenes.push_back(move(pScene));
	}
	void FrameAdvance();

	bool IsQuitable() const { return m_bMainWindow; }
	void SetMain() { m_bMainWindow = true; }

	void ChangeScene(std::wstring Tag, bool bDestroyPostScene = false);
	CScene* FindScene(std::wstring Tag);

	// Framework 에서 호출하는 윈도우 프로시저입니다.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

};
