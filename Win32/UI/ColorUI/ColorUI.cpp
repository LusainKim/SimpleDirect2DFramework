#include "stdafx.h"
#include "UI/UIManager.h"
#include "Framework/Framework.h"
#include "Scene/Title/TitleScene.h"
#include "ColorUI.h"



CColorUI::~CColorUI()
{
	if (m_pColorBuffer) delete[] m_pColorBuffer;
}

void CColorUI::Draw(ID2D1RenderTarget * pd2dRenderTarget)
{
	ComPtr<ID2D1SolidColorBrush> brush;
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::White, 1.f }, &brush);
	pd2dRenderTarget->DrawRectangle(GetRect(UI_PROPERTY::Client) + RectF(-1, -1, 1, 1), brush.Get());
	pd2dRenderTarget->DrawBitmap(m_pd2dbmpColors.Get(), GetRect(UI_PROPERTY::Client));
}

void CColorUI::OnPrepareRender(ID2D1RenderTarget * pd2dRenderTarget)
{
	auto sz = GetRelatedStartPosition();
//	sz.width -= 1;
//	sz.width -= 1;
	auto rc = GetRelatedDrawableRect() + RectF(-1, -1, 1, 1);
	pd2dRenderTarget->GetTransform(&m_mtxStoreLast);
	pd2dRenderTarget->SetTransform(Matrix3x2F::Translation(sz));
	pd2dRenderTarget->PushAxisAlignedClip(rc, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void CColorUI::OnFinishedRender(ID2D1RenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->PopAxisAlignedClip();
	pd2dRenderTarget->SetTransform(m_mtxStoreLast);
}

void CColorUI::Update(float fTimeElapsed)
{
	if (m_SelectedColor.CheckNotify())
	{
		MakeColorField();
	}
}

void CColorUI::MakeColorField()
{
	size_t width = static_cast<size_t>(m_rcClient.right - m_rcClient.left);
	size_t height = static_cast<size_t>(m_rcClient.bottom - m_rcClient.top);

	if (m_pColorBuffer) delete[] m_pColorBuffer;
	m_pColorBuffer = reinterpret_cast<Color*>(std::malloc(width * height * sizeof(Color)));

	Color hsv{ m_SelectedColor->h };

	// v val per step. [0.0, 1.0)
	float stepW = 1.f / static_cast<float>(width - 1);
	float stepH = 1.f / static_cast<float>(height - 1);

	for (int y = height - 1; y >= 0; --y)
	{
		// s val per step. (0.0, 1.0]
		hsv.s = 0.f;

		for (int x = 0; x < width; ++x)
		{
			m_pColorBuffer[(y * width) + x] = Color::HSVtoRGB(hsv);
			hsv.s += stepW;
		}

		hsv.v += stepH;
	}

	auto rendertarget = m_pUIManager->GetSceneInfo()->framework->GetRenderTarget();

	ComPtr<ID2D1Bitmap> bmp;
	auto rc = RectU(0, 0, width, height);
	rendertarget->CreateBitmap(SizeU(width, height)
		, BitmapProperties(PixelFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, D2D1_ALPHA_MODE_PREMULTIPLIED))
		, &bmp);

	bmp.As(&m_pd2dbmpColors);

	m_pd2dbmpColors->CopyFromMemory(&rc, m_pColorBuffer, width * sizeof(Color));
}

void CColorUI::Build(std::string Tag, const shared_ptr<CIndRes>& indres, ID2D1RenderTarget* pd2dRenderTarget)
{
	Base::Build(Tag, indres, pd2dRenderTarget);
	MakeColorField();
}
