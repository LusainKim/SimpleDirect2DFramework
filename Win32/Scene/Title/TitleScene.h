#pragma once

#include "Scene\Scene.h"



class CTitleScene
	: public CScene
{
public:

	CTitleScene();
	virtual ~CTitleScene() = default;
	
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual void BuildObjects(std::wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework) override;
	virtual void ReleaseObjects() override;

	virtual void AnimateObjects(float fTimeElapsed) override;

	virtual void BuildObjecsFromD2D1Devices(  ID2D1Factory          * pd2dDevice
											, ID2D1HwndRenderTarget * pd2dDeviceContext
											, IDWriteFactory2       * pdwFactory
											, IWICImagingFactory2   * pwicFactory
	) override;

	virtual void Update2D(float fTimeElapsed) override;
	virtual void Render2D(ID2D1HwndRenderTarget*pd2dDeviceContext) override;

private:
	float m_fTick{ 0.f };
};

