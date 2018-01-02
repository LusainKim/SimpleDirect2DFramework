#include "stdafx.h"
#include "Framework/Framework.h"
#include "Framework/Input/Input.h"
#include "TitleScene.h"



CTitleScene::CTitleScene()
{
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CTitleScene::Build(std::string Tag, CDirectXFramework * pMasterFramework)
{
	CScene::Build(Tag, pMasterFramework);
}

void CTitleScene::ReleaseObjects()
{
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
}

void CTitleScene::Draw(ID2D1HwndRenderTarget * pd2dDeviceContext)
{
	ComPtr<ID2D1SolidColorBrush> hbr;
	pd2dDeviceContext->CreateSolidColorBrush(ColorF{ ColorF::AliceBlue }, &hbr);

	float angle = XMConvertToRadians(m_fTick * 270.f);
	float x = 50 + cos(angle) * 25.f;
	float y = 50 + sin(angle) * 25.f;
	pd2dDeviceContext->FillRectangle(RectF(x - 10.f, y - 10.f, x + 10.f, y + 10.f), hbr.Get());
}

void CTitleScene::BindKey()
{
	using BTN = CInputManager::Button;
	using namespace InputManager;

	make_button(move_left);
	move_left.down_event;
}
