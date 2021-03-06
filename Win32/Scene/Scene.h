//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

class CDirectXFramework;
class CInputManager;


class CSceneBase
{
public:

	CSceneBase() = default;

	virtual ~CSceneBase();
	virtual void ReleaseObjects() {}

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BindKey() {}
	virtual void ProcessInput(bool(&arrByte)[256]);

	virtual void AnimateObjects(float fTimeElapsed) {}

	virtual void EnterScene() {}
	virtual void LeaveScene() {}

protected:
	std::string			m_Tag;

public:
	bool FindByTag(const std::string& Tag) const { return Tag == tag; }
	const std::string&	tag		{ m_Tag }	;

protected:
	HWND						m_hWnd				;
	shared_ptr<CInputManager>	m_InputManager		;
};



class CScene
	: public CSceneBase
{
public:

	CScene() = default;
	virtual ~CScene() = default;

	virtual void Build(std::string Tag, CDirectXFramework *pMasterFramework);
	virtual void Draw(ID2D1HwndRenderTarget * pd2dDeviceContext) {}
	
protected:
	shared_ptr<CIndRes>					m_pIndRes			{ nullptr }	;
	CDirectXFramework				*	m_pMasterFramework				;

public:
	CDirectXFramework * const &			framework{ m_pMasterFramework }	;


};

