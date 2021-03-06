//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework/Framework.h"
#include "Framework/Input/Input.h"
#include "Scene.h"



CSceneBase::~CSceneBase()
{
	if (m_InputManager) m_InputManager->SaveAllKey(tag.c_str(), "./config.ini");
	ReleaseObjects();
}

bool CSceneBase::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	default:
		return true;
	}

	return false;
}

bool CSceneBase::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	default:
		return true;
	}

	return false;
}

void CSceneBase::ProcessInput(bool(&arrByte)[256])
{
	if (!m_InputManager) return;
	m_InputManager->ProcessInput(arrByte); 
}

bool CSceneBase::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	default:
		return true;
	}

	return false;
}



void CScene::Build(string Tag, CDirectXFramework *pMasterFramework)
{
	m_Tag = Tag;
	m_pMasterFramework = pMasterFramework;
	m_hWnd = m_pMasterFramework->GetHwnd();
	m_pIndRes = m_pMasterFramework->IndependentResources();

	if (!m_InputManager)
	{ 
		m_InputManager = make_shared<CInputManager>();
		BindKey();
	}
}
