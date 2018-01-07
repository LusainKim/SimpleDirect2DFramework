#pragma once

class CUIManager;

enum class UI_PROPERTY
{
	  Caption
	, Client
	, StartPosition
	, Resize
};

class CUIBase
{

public:

	CUIBase(CUIManager * const pUIManager);
	virtual ~CUIBase() = default;

	virtual void RenderCaption(ID2D1RenderTarget* pd2dRenderTarget) {}
	virtual void RenderResizeBox(ID2D1RenderTarget* pd2dRenderTarget) {}
	virtual void OnPrepareRender(ID2D1RenderTarget* pd2dRenderTarget);
	virtual void Render(ID2D1RenderTarget* pd2dRenderTarget) = 0;
	virtual void OnFinishedRender(ID2D1RenderTarget* pd2dRenderTarget) { pd2dRenderTarget->PopLayer(); }
	virtual void Update(float fTimeElapsed) {}

	void IsRelatedClientSize(bool bTrue) { m_bRelatedClient = bTrue; }

	D2D_SIZE_F GetRelatedStartPosition() const;
	D2D_RECT_F GetRelatedDrawableRect() const;


	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	template<typename... Args>
	void SetProperties(UI_PROPERTY prop, Args&&... args)
	{
		switch (prop)
		{
		case UI_PROPERTY::Caption:
			InitializeCaption(std::forward<Args>(args)...);
			break;

		case UI_PROPERTY::Client:
			InitializeClinet(std::forward<Args>(args)...);
			break;

		case UI_PROPERTY::StartPosition:
			InitializeClinet(std::forward<Args>(args)...);
			break;

		case UI_PROPERTY::Resize:
			InitializeResize(std::forward<Args>(args)...);
			break;
		}
	}

	void InitializeCaption(float fHeight, bool bView = true, bool bMoveable = true);
	void InitializeCaption(D2D_RECT_F rc, bool bView = true, bool bMoveable = true);
	void InitializeClinet(D2D_RECT_F rc, bool bVisible = true);
	void InitializeResize(D2D_RECT_F rc = D2D_RECT_F { -20, -20, 0, 0 }, bool bVisible = true);
	void InitializeStartPosition(D2D_POINT_2F pt) { m_ptWorld = pt; }

	D2D_RECT_F GetRect(UI_PROPERTY prop) const;

	D2D_RECT_F GetUIRect() const { return MergeRect(m_rcClient, m_rcCaption) + m_ptWorld; }
	D2D_RECT_F GetClientRect() const { return m_rcClient + m_ptWorld; }
	D2D_RECT_F GetCaptionRect() const { return !m_bCaption ? D2D_RECT_F {} : (m_rcCaption + m_ptWorld); }
	D2D_RECT_F GetResizeRect() const { return !m_bResize ? D2D_RECT_F {} : (Point2F(m_rcClient.right, m_rcClient.bottom) + m_ptWorld + m_rcResizeBox); }

	bool AbleMove(bool bAble = true) { if (!m_bCaption) return false; return (m_bAbleMove = bAble); }
	bool AbleResize(bool bAble = true) { if (!m_bResize) return false; return (m_bAbleResize = bAble); }

	bool SetVisible(bool b = true) { if (!m_bValid) return false; return (m_bVisible = b); }

	void Move(POINT pt) { if (!m_bAbleMove) return; m_ptWorld = m_ptWorld + Point2F(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y)); }
	void Resize(POINT pt, float minWidth = 20.f, float minHeight = 10.f)
	{
		if (!m_bAbleResize) return; 
		m_rcClient.right += pt.x;
		m_rcClient.bottom += pt.y; 

		m_rcClient.right = max(minWidth, m_rcClient.right);
		m_rcClient.bottom = max(minHeight, m_rcClient.bottom);

		if (!m_bCustomCaption)
		{
			m_rcCaption.left = m_rcClient.left;
			m_rcCaption.right = m_rcClient.right;
		}
	}

	// 동일한 함수
	bool IsValid() const { return m_bValid && m_bVisible; }
	bool IsVisible() const { return m_bValid && m_bVisible; }

	bool PtInUI(POINT pt)		const { RECT rc = D2DRectToWin32Rect(GetUIRect());		return (TRUE == PtInRect(&rc, pt)); }
	bool PtInClient(POINT pt)	const { RECT rc = D2DRectToWin32Rect(GetClientRect());	return (TRUE == PtInRect(&rc, pt)); }
	bool PtInCaption(POINT pt)	const { RECT rc = D2DRectToWin32Rect(GetCaptionRect());	return (TRUE == PtInRect(&rc, pt)); }
	bool PtInResize(POINT pt)	const { RECT rc = D2DRectToWin32Rect(GetResizeRect());	return (TRUE == PtInRect(&rc, pt)); }

	void SetOpacity(float fOpacity) { m_fOpacity = fOpacity; }
	float SetOpacity() const { return m_fOpacity; }

	virtual void BuildObject(string_hash hash, ID2D1Factory* pd2dFactory, IDWriteFactory* pdwFactory, IWICImagingFactory* pwicFactory, ID2D1DeviceContext* pd2dDeviceContext) = 0
	{
		m_myHash = hash;
		pd2dDeviceContext->CreateLayer(&m_pLayer);
	}

	bool FindByTag(string_hash hash) const { return hash == m_myHash; };
	string_hash GetTag() const { return m_myHash; }
	
protected:

	CUIManager * const			m_pUIManager		{ nullptr }	;

	string_hash					m_myHash			{ }			;

	bool						m_bAbleResize		{ false }	;
	bool						m_bAbleMove			{ false }	;
								
	// Active					
	bool						m_bVisible			{ false }	;
	bool						m_bValid			{ false }	;
	bool						m_bCaption			{ false }	;
	bool						m_bResize			{ false }	;
	bool						m_bClient			{ false }	;
			
	bool						m_bRelatedClient	{ false }	;

	bool						m_bCustomCaption	{ false }	;
	D2D_RECT_F					m_rcCaption			{ }			;
	D2D_RECT_F					m_rcClient			{ }			;
													
	D2D_RECT_F					m_rcResizeBox		{ }			;

	D2D_POINT_2F				m_ptWorld			{ }			;

	// Layer
	ComPtr<ID2D1Layer>			m_pLayer						;

	float						m_fOpacity			{ 1.f }		;

};


/// <summary>
/// 실제 사용하지는 않지만 이런 식으로 구현된다는 것을 보여주는 클래스입니다.
/// </summary>
class CSimpleUI
	: public CUIBase
{
	using Base = CUIBase;

public:

	CSimpleUI(CUIManager * const pUIManager)
		: CUIBase(pUIManager) 
	{}
	virtual ~CSimpleUI() = default;

	void RenderCaption(ID2D1RenderTarget* pd2dRenderTarget) override
	{
		pd2dRenderTarget->PushLayer(
			LayerParameters(
		//		  InfiniteRect()
				  m_rcCaption
				, nullptr
				, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
				, Matrix3x2F::Identity()
				, 1.f
				, nullptr
				, D2D1_LAYER_OPTIONS_NONE
			)
			, m_LayerCaption.Get()
		);

		pd2dRenderTarget->FillRectangle(m_rcCaption, m_hbrCaption.Get());

		auto rcTitle = m_rcCaption + Point2F(15, 0);

		wstring str = TEXT("UI Sample");
		pd2dRenderTarget->DrawText(
			  str.c_str()
			, static_cast<UINT>(str.length())
			, m_pdwTextFormat.Get()
			, rcTitle
			, m_hbrBackground.Get()
		);

		pd2dRenderTarget->PopLayer();
	}
	
	void RenderResizeBox(ID2D1RenderTarget* pd2dRenderTarget) override
	{
		pd2dRenderTarget->FillRectangle(GetResizeRect() - m_ptWorld, m_hbrCaption.Get());
	}

	void Render(ID2D1RenderTarget* pd2dRenderTarget) override
	{
		if (m_bCaption) RenderCaption(pd2dRenderTarget);
		pd2dRenderTarget->FillRectangle(m_rcClient, m_hbrBackground.Get());
		pd2dRenderTarget->FillRectangle(RectF(25,25,100,100), m_hbrSample.Get());
		if (m_bResize) RenderResizeBox(pd2dRenderTarget);
	}

	void Update(float fTimeElapsed) override
	{

	}

	void BuildObject(string_hash hash, ID2D1Factory* pd2dFactory, IDWriteFactory* pdwFactory, IWICImagingFactory* pwicFactory, ID2D1DeviceContext* pd2dDeviceContext) override
	{
		Base::BuildObject(hash, pd2dFactory, pdwFactory, pwicFactory, pd2dDeviceContext);

		pd2dDeviceContext->CreateSolidColorBrush(ColorF { ColorF::Black }, &m_hbrSample);
		pd2dDeviceContext->CreateSolidColorBrush(ColorF { ColorF::BlanchedAlmond }, &m_hbrBackground);
		pd2dDeviceContext->CreateSolidColorBrush(ColorF { ColorF::Crimson }, &m_hbrCaption);
		// Create Text
		ComPtr<IDWriteTextFormat> txtformat;
		pdwFactory->CreateTextFormat(	  TEXT("consolas")
										, nullptr
										, DWRITE_FONT_WEIGHT_NORMAL
										, DWRITE_FONT_STYLE_NORMAL
										, DWRITE_FONT_STRETCH_NORMAL
										, 12
										, L"ko-kr"
										, &txtformat
		);

		txtformat.As(&m_pdwTextFormat);
		m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
		pd2dDeviceContext->CreateLayer(&m_LayerCaption);
	}

protected:

	ComPtr<IDWriteTextFormat1>			m_pdwTextFormat;
	ComPtr<ID2D1SolidColorBrush>		m_hbrSample;
	ComPtr<ID2D1SolidColorBrush>		m_hbrBackground;
	ComPtr<ID2D1SolidColorBrush>		m_hbrCaption;
	ComPtr<ID2D1Layer>					m_LayerCaption;
};


