//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

class CDirectXFramework;



class CSceneBase
{
public:

	CSceneBase() = default;
	virtual ~CSceneBase();

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void ReleaseObjects() {}

	virtual void AnimateObjects(float fTimeElapsed) {}

	bool FindByTag(std::wstring Tag);
	std::wstring GetTag() const { return m_wsTag; }

protected:
	std::wstring						m_wsTag				;
	HWND								m_hWnd				;
};

class CScene
	: public CSceneBase
{
public:

	CScene() = default;
	virtual ~CScene() = default;

	virtual void BuildObjects(std::wstring Tag, HWND hWnd, CDirectXFramework *pMasterWindow);
	virtual void BuildObjecsFromD2D1Devices(  ID2D1Factory          * pd2dFactory
											, ID2D1HwndRenderTarget * pd2dRenderTarget
											, IDWriteFactory2       * pdwFactory
											, IWICImagingFactory2   * pwicFactory
	) {}

	virtual void Update2D(float fTimeElapsed) {}
	virtual void Render2D(ID2D1HwndRenderTarget*pd2dDeviceContext) {}
	
protected:
	CDirectXFramework				*	m_pMasterFramework	;

};

