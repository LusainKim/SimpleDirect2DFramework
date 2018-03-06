#pragma once

#include "UI/UIBase.h"

class CCustomCaptionUI
	: public CUIBase
{
	using Base = CUIBase;

public:

	CCustomCaptionUI(CUIManager * const pUIManager)
		: CUIBase{ pUIManager }
	{
	}
	virtual ~CCustomCaptionUI();

	void Draw(ID2D1RenderTarget* pd2dRenderTarget) override;
	void OnPrepareRender(ID2D1RenderTarget* pd2dRenderTarget) override;
	void OnFinishedRender(ID2D1RenderTarget* pd2dRenderTarget) override;
	void Update(float fTimeElapsed) override;
	void Build(std::string Tag, const shared_ptr<CIndRes>& indres, ID2D1RenderTarget* pd2dRenderTarget) override;

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)	override;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)		override;
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)	override;

protected:

	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrBackground;
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrCaptionText;
	ComPtr<ID2D1Bitmap>				m_pd2dbmpIcon;
	ComPtr<IDWriteTextFormat>		m_tfCaption;
	ComPtr<IDWriteTextLayout>		m_tlCaption;

	float							m_fIconSize;
	D2D_RECT_F						m_rcIcon;
};
