//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework/Framework.h"
#include "Framework/Input/Input.h"
#include "Scene.h"



CSceneBase::~CSceneBase()
{
	ReleaseObjects();
}

bool CSceneBase::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}

	return(true);
}

bool CSceneBase::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	default:
		return true;
	}

	return true;
}

void CSceneBase::ProcessInput(bool(&arrByte)[256])
{
	if (!m_InputManaget) return;
	m_InputManaget->ProcessInput(arrByte); 
}

bool CSceneBase::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(true);
}



void CScene::Build(string Tag, CDirectXFramework *pMasterFramework)
{
	m_Tag = Tag;
	m_pMasterFramework = pMasterFramework;
	m_hWnd = m_pMasterFramework->GetHwnd();

	if (!m_InputManaget)
	{ 
		m_InputManaget = make_shared<CInputManager>();
		BindKey();
	}
}
