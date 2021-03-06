#include "stdafx.h"
#include "UIBase.h"
#include "UIManager.h"

CUIManager::CUIManager(CScene * const pScene)
	: m_pScene { pScene }
{

}

bool CUIManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:

		m_ptCurrent = POINT { LOWORD(lParam), HIWORD(lParam) };

		// seq 1: empty selected - select ui
		// seq 2: If nothing is selected after seq 1, control is returned to the scene.
		if (!m_pSelectedUI && !SelectValidUI()) return false;
		
		if (m_pSelectedUI->PtInCaption(m_ptCurrent))
		{
			m_ptOld = m_ptCurrent;
			m_bDragResize = false;
			m_bDragCaption = true;
		}
		else if (m_pSelectedUI->PtInResize(m_ptCurrent))
		{
			m_ptOld = m_ptCurrent;
			m_bDragCaption = false;
			m_bDragResize = true;
		}
		else if (m_pSelectedUI->PtInClient(m_ptCurrent))
			return m_pSelectedUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		// clear select
		else
		{
			m_pSelectedUI = nullptr;
			return false;
		}
		
		break;

	case WM_MOUSEMOVE:

		if (!m_pSelectedUI) return false;

		if (!(m_bDragCaption || m_bDragResize))
			return m_pSelectedUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

		m_ptCurrent = POINT { LOWORD(lParam), HIWORD(lParam) };

		if (m_bDragCaption)
		{
			m_pSelectedUI->Move(m_ptCurrent - m_ptOld);
		}
		else if (m_bDragResize)
		{
			m_pSelectedUI->Resize(m_ptCurrent - m_ptOld);
		}

		m_ptOld = m_ptCurrent;
				
		break;

	case WM_LBUTTONUP:
		{
			bool retval = (m_bDragCaption || m_bDragResize);
			m_bDragCaption = false;
			m_bDragResize = false;
			if (!retval) return false;
			else return m_pSelectedUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		}

	case WM_RBUTTONDOWN:

		m_ptCurrent = POINT{ LOWORD(lParam), HIWORD(lParam) };

		// seq 1: empty selected - select ui
		// seq 2: If nothing is selected after seq 1, control is returned to the scene.
		if (!m_pSelectedUI && !SelectValidUI()) return false;
		
		if (m_pSelectedUI->PtInUI(m_ptCurrent))
			return m_pSelectedUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		// clear select
		else
		{
			m_pSelectedUI = nullptr;
			return false;
		}
		
		break;

	case WM_RBUTTONUP:
		if (m_pSelectedUI)
			return m_pSelectedUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return false;

	default:
		return false;
	}

	return(true);
}

bool CUIManager::SelectValidUI()
{
	auto pSelectedUI = std::find_if(begin(m_lstUI), end(m_lstUI) 
		, [&](const unique_ptr<CUIBase>& p)
			{ return p->PtInUI(m_ptCurrent); }
	);
	if (end(m_lstUI) != pSelectedUI)
	{
		m_pSelectedUI = pSelectedUI->get();
		return true;
	}
	return false;
}

bool CUIManager::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		return OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		return  OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	case WM_SIZE:
		for (auto& p : m_lstUI)
			p->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
		return false;
	default:
		return false;
	}

	return true;
}

void CUIManager::Delete(std::string tag) 
{ 
	m_lstUI.remove_if(
		[&](const auto& p) 
		{ 
			return p->GetTag() == tag; 
		}
	); 
}

CUIBase* CUIManager::find(std::string tag)
{
	auto res = find_if(m_lstUI.begin(), m_lstUI.end(), [&] (const auto& p) { return p->GetTag() == tag; });
	return (res == end(m_lstUI) ? nullptr : res->get());
}

bool CUIManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	default:
		return false;
	}
	return(true);
}

void CUIManager::Draw(ID2D1RenderTarget * pd2dRenderTarget)
{
	for (const auto& p : m_lstUI)
	{
		p->OnPrepareRender(pd2dRenderTarget);

		p->Draw(pd2dRenderTarget);
		
		p->OnFinishedRender(pd2dRenderTarget);
	}
}

void CUIManager::Update(float fTimeElapsed)
{
	for (const auto& p : m_lstUI)
		p->Update(fTimeElapsed);
}
