
#include "stdafx.h"
#include "UIBase.h"
#include "UIManager.h"
#include "Scene/Scene.h"
#include "Framework/Framework.h"

CUIBase::CUIBase(CUIManager * const pUIManager)
	: m_pUIManager { pUIManager }
{

}

D2D_SIZE_F CUIBase::GetRelatedStartPosition() const
{
	auto szStart { SizeF(m_ptWorld.x, m_ptWorld.y) };
	if (m_bRelatedClient)
	{
		auto sz = m_pUIManager->GetSceneInfo()->framework->GetClientSize();
		szStart.width  *= sz.width;
		szStart.height *= sz.height;
	}
	return szStart;
}

D2D_RECT_F CUIBase::GetRelatedDrawableRect() const
{
	auto rcDraw { MergeRect(m_rcClient, m_rcCaption) };

	if (m_bRelatedClient)
	{
		auto sz = m_pUIManager->GetSceneInfo()->framework->GetClientSize();
//		auto aspect = sz.height / sz.width;

		rcDraw.left   *= sz.width;

		if (rcDraw.right == 0.f)
			rcDraw.right = rcDraw.bottom * sz.height;
		else
			rcDraw.right  *= sz.width;

		rcDraw.top *= sz.height;

		if (rcDraw.bottom == 0.f)
			rcDraw.bottom = rcDraw.right;// *sz.height;
		else
			rcDraw.bottom *= sz.height;
	}
	return rcDraw;
}

void CUIBase::OnPrepareRender(ID2D1RenderTarget * pd2dRenderTarget)
{
	GetRelatedDrawableRect();

	pd2dRenderTarget->GetTransform(&m_mtxStoreLast);
	pd2dRenderTarget->SetTransform(Matrix3x2F::Translation(GetRelatedStartPosition()));
	pd2dRenderTarget->PushLayer(
		LayerParameters(
	//		  InfiniteRect()
			  GetRelatedDrawableRect()
			, nullptr
			, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
			, Matrix3x2F::Identity()
			, m_fOpacity
			, nullptr
			, D2D1_LAYER_OPTIONS_NONE
		)
		, m_pLayer.Get()
	);
}

void CUIBase::OnFinishedRender(ID2D1RenderTarget * pd2dRenderTarget) 
{ 
	pd2dRenderTarget->PopLayer(); 
	pd2dRenderTarget->SetTransform(m_mtxStoreLast);
}

void CUIBase::InitializeCaption(float fHeight, bool bView, bool bMoveable)
{
	m_bCustomCaption = false;
	m_rcCaption.top = -fHeight; m_rcCaption.bottom = 0;
	if (m_bClient)
	{
		m_rcCaption.left = m_rcClient.left;
		m_rcCaption.right = m_rcClient.right;
	}

	m_bCaption = bView; m_bAbleMove = bMoveable;
}

void CUIBase::InitializeCaption(D2D_RECT_F rc, bool bView, bool bMoveable)
{
	m_bCustomCaption = true;
	m_rcCaption = rc;

	m_bCaption = bView; m_bAbleMove = bMoveable;
}

void CUIBase::InitializeClinet(D2D_RECT_F rc, bool bVisible)
{
	m_rcClient = rc;
	if (!m_bCustomCaption)
	{
		m_rcCaption.left = m_rcClient.left;
		m_rcCaption.right = m_rcClient.right;
	}
	m_bValid = true;
	m_bClient = true;
	m_bVisible = bVisible;
}

void CUIBase::InitializeResize(D2D_RECT_F rc, bool bVisible)
{
	m_rcResizeBox = rc;
	m_bResize = true;
	m_bAbleResize = bVisible;
}

D2D_RECT_F CUIBase::GetRect(UI_PROPERTY prop) const
{
	switch (prop)
	{
	case UI_PROPERTY::Caption:
		return m_rcCaption;
	case UI_PROPERTY::Client:
		return m_rcClient;
	case UI_PROPERTY::Resize:
		return m_rcResizeBox;
	default: ;
	}

	return D2D_RECT_F {};
}

void CUIBase::Build(std::string Tag, const shared_ptr<CIndRes>& indres, ID2D1RenderTarget* pd2dRenderTarget)
{
	m_myTag = Tag;
	pd2dRenderTarget->CreateLayer(&m_pLayer);
}

bool CUIBase::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		return false;

		break;

	case WM_MOUSEMOVE:
		return false;

		break;

	case WM_LBUTTONUP:
		return false;
		break;

	default:
		return false;
	}

	return(true);
}

bool CUIBase::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
//	switch (nMessageID)
//	{
//	case WM_SIZE:
//		break;
//	default:
//		return false;
//	}
//
//	return true;
}

bool CUIBase::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		return false;
	}
	return(true);
}