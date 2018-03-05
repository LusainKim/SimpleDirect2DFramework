#pragma once

class CScene;
class CUIBase;

class CUIManager
{
public:
	
	CUIManager(CScene* const pScene);
	~CUIManager() = default;

	void Insert(unique_ptr<CUIBase>&& ui) { m_lstUI.push_back(move(ui)); }
	void Delete(std::string tag);
	CUIBase* find(std::string tag);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool SelectValidUI();
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void Draw(ID2D1RenderTarget* pd2dRenderTarget);
	void Update(float fTimeElapsed);

	CScene* GetSceneInfo() const { return m_pScene; }

private:

	CScene* const	 				m_pScene;

	list<unique_ptr<CUIBase>>		m_lstUI;
	CUIBase*						m_pSelectedUI = nullptr;

	POINT							m_ptCurrent;
	POINT							m_ptOld;
	bool							m_bDragCaption = false;
	bool							m_bDragResize = false;
};