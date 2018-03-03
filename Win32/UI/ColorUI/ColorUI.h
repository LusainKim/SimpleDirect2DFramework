#pragma once

#include "UI/UIBase.h"

class CColorUI
	: public CUIBase
{
	using Base = CUIBase;

public:

	CColorUI(CUIManager * const pUIManager, Notifier<Color>& color)
		: CUIBase			{ pUIManager }
		, m_SelectedColor	{ color }
	{}
	virtual ~CColorUI();

	void Draw(ID2D1RenderTarget* pd2dRenderTarget) override;

	void Update(float fTimeElapsed) override;
	void MakeColorField();
	void Build(std::string Tag, const shared_ptr<CIndRes>& indres, ID2D1RenderTarget* pd2dRenderTarget) override;

protected:

	Notifier<Color>					&	m_SelectedColor;
	// (width * y + x)
	Color							*	m_pColorBuffer	{ nullptr };

	ComPtr<ID2D1Bitmap1>				m_pd2dbmpColors;


};
