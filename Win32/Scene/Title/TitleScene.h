#pragma once

#include "Scene/Scene.h"



class CTitleScene
	: public CScene
{
public:

	CTitleScene();
	virtual ~CTitleScene() = default;
	virtual void ReleaseObjects() override;

	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual void Build(std::string Tag, CDirectXFramework *pMasterWindow) override;

	void BuildHSVPicker();

	void BuildColorPicker();

	virtual void AnimateObjects(float fTimeElapsed) override;

	virtual void Draw(ID2D1HwndRenderTarget*pd2dDeviceContext) override;

	virtual void BindKey() override;

private:
	float m_fTick{ 0.f };
	ComPtr<ID2D1Bitmap1> m_pd2dbmpColors;
	ComPtr<ID2D1Bitmap1> m_pd2dbmpColorsPallet;

	float m_Hvalue = 0.f;
};

