#pragma once

class CScene;
class CUIBase;

class CUIManager
{
public:
	
	CUIManager(CScene* const pScene);
	~CUIManager() = default;

	void Insert(unique_ptr<CUIBase>&& ui) { m_lstUI.push_back(move(ui)); }
	void Delete(string_hash hash) { m_lstUI.remove_if([&] (const auto& p) { return p->GetTag() == hash; }); }
	CUIBase* find(string_hash hash);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void Render(ID2D1RenderTarget* pd2dRenderTarget);
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