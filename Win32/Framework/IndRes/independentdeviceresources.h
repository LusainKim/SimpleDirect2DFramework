#pragma once

#define USING_D3D11_INDRES	0
#define USING_D2D1_INDRES	1

//	2017.03.26
//	���� ���� :
//		Getter ����
//		Initialzer�� main���� ����
//		���� ������ �����ص� ���� ���� ������ �ʹ�. �� Singleton ?
//		dependent resource create �Լ� ����?

#if USING_D3D11_INDRES

/// <summary>
/// DirectX11�� ȣȯ�Ǵ� ��ġ ������ ���ҽ��� �����մϴ�.
///
/// ��ġ �������� ���ҽ� ������ ������ �� �Լ��� �����մϴ�.
///
/// ���� ���ҽ��� �����ϰ� �����մϴ� :
///		ID3D11Device
///		ID3D11DeviceContext
///
/// </summary>
class CDirect3DIndependentDeviceResource
{
public:

	CDirect3DIndependentDeviceResource();
	virtual ~CDirect3DIndependentDeviceResource();

	/// <summary>
	/// ��ġ ������ ����̽��� �����մϴ�.
	/// </summary>
	///	<param name = "bCreate2DResources"> Direct2D ���ҽ��� �������� �����մϴ�. �⺻���� true �Դϴ�. </param>
	virtual bool Initialize();

	/// <summary>
	/// Swap Chain ��ü�� �����Ͽ� ��ȯ�մϴ�.
	/// </summary>
	///	<param name = "hWnd"> Swap Chain ��ü�� ����� ������ �ڵ��Դϴ�. </param>
	///	<param name = "dxgiFormat"> 
	/// <para> Swap Chain ��ü�� ����� DXGI_FORMAT ���Դϴ�.</para> 
	/// <para> �� ���� ����ؾ� ResizeBuffer ���� �Լ����� ��Ȯ�� ���� ���� �� �ֽ��ϴ�.</para> 
	/// </param>
	ComPtr<IDXGISwapChain> CreateSwapChain(HWND hWnd, DXGI_FORMAT dxgiFormat) const;

	/// <summary>
	/// Swap Chain ��ü�� �����Ͽ� ��ȯ�մϴ�.
	/// </summary>
	///	<param name = "desc"> Swap Chain ��ü�� �����ϴ� �� ���̴� ����ü�Դϴ�. </param>
	ComPtr<IDXGISwapChain> CreateSwapChain(DXGI_SWAP_CHAIN_DESC desc) const;

	/// <summary>
	/// Swap Chain�� ���ӵ� RenderTargetView�� �����մϴ�.
	/// </summary>
	///	<param name = "pdxgiSwapChain"> Swap Chain ��ü�Դϴ�. </param>
	/// <remarks> ��ȯ�Ǵ� RTV�� �ٸ� Swap Chain ��ü���� ������ RTV�� ȣȯ���� �ʽ��ϴ�. </remarks>
	ComPtr<ID3D11RenderTargetView> CreateRenderTargetViewBySwapChain(IDXGISwapChain* pdxgiSwapChain) const;

	/// <summary>
	/// Depth-Stencil Buffer�� ����ϴ�.
	/// </summary>
	///	<param name = "rc"> Render Target View�� ������ ũ���Դϴ�. </param>
	/// <remarks> ũ�Ⱑ �ٸ��� D3D WARNNING �� �߻��մϴ�. </remarks>
	ComPtr<ID3D11Texture2D> CreateDepthStencilBuffer(RECT rc) const;

	/// <summary>
	/// Depth-Stencil Buffer�� ����ϴ�.
	/// </summary>
	///	<param name = "desc"> Buffer�� ����� ���� Texture2D Description�Դϴ�. </param>
	/// <remarks> Render Target View�� ũ�Ⱑ �ٸ��� D3D WARNNING �� �߻��մϴ�. </remarks>
	ComPtr<ID3D11Texture2D> CreateDepthStencilBuffer(D3D11_TEXTURE2D_DESC desc) const;

	/// <summary>
	/// Depth-Stencil View�� ����ϴ�.
	/// </summary>
	///	<param name = "pd3dDepthStencilBuffer"> Depth-Stencil Buffer �Դϴ�. </param>
	ComPtr<ID3D11DepthStencilView> CreateDepthStencilView(ID3D11Texture2D* pd3dDepthStencilBuffer) const;

	// Getter
public:
	auto d3dDevice()		const { return m_pd3dDevice.Get(); }
	auto d3dDeviceContext()	const { return m_pd3dDeviceContext.Get(); }

	auto COM_d3dDevice()		const { return m_pd3dDevice; }
	auto COM_d3dDeviceContext()	const { return m_pd3dDeviceContext; }

	// Direct3D Resource
protected:

	ComPtr<ID3D11Device2>			m_pd3dDevice		{ nullptr }	;
	ComPtr<ID3D11DeviceContext2>	m_pd3dDeviceContext	{ nullptr }	;

	ComPtr<IDXGIFactory3>			m_dxgiFactory		{ nullptr }	;
	ComPtr<IDXGIDevice3>			m_dxgiDevice		{ nullptr }	;
	ComPtr<IDXGIAdapter2>			m_dxgiAdapter		{ nullptr }	;

};

#endif



#if USING_D2D1_INDRES

/// <summary>
/// Direct2D�� ȣȯ�Ǵ� ��ġ ������ ���ҽ��� �����մϴ�.
///
/// ��ġ �������� ���ҽ� ������ ������ �� �Լ��� �����մϴ�.
///
/// ���� ���ҽ��� �����ϰ� �����մϴ� :
///		ID2D1Factory
///		IDIDWriteFactory
///		IWICImagingFactory
///
/// </summary>
class CDirect2DIndependentDeviceResource
{

public:

	CDirect2DIndependentDeviceResource();
	~CDirect2DIndependentDeviceResource();

	/// <summary>
	/// ��ġ ������ ����̽��� �����մϴ�.
	/// </summary>
	///	<param name = "bCreate2DResources"> Direct2D ���ҽ��� �������� �����մϴ�. �⺻���� true �Դϴ�. </param>
	bool Initialize();

	/// <summary>
	/// ������ �ڵ鿡 ���ӵǴ� Render Target�� �����մϴ�.
	/// </summary>
	///	<param name = "bCreate2DResources"> Render Target�� ����� ������ �ڵ��Դϴ�. </param>
	///	<param name = "prop"> Render Target �Ӽ����Դϴ�. �⺻���� RenderTargetProperties() �Դϴ�. </param>
	///	<param name = "presentOpt"> Render Target�� ��� �ɼ��Դϴ�. �⺻���� D2D1_PRESENT_OPTIONS_IMMEDIATELY �Դϴ�. </param>
	ComPtr<ID2D1HwndRenderTarget> CreateHwndRenderTarget(HWND hWnd, D2D1_RENDER_TARGET_PROPERTIES prop = D2D1::RenderTargetProperties(), D2D1_PRESENT_OPTIONS presentOpt = D2D1_PRESENT_OPTIONS_IMMEDIATELY) const;

	// Getter
public:

	auto d2dFactory()		const { return m_pd2dFactory.Get(); }
	
	auto dwFactory()		const { return m_pdwFactory.Get(); }
	auto wicFactory()		const { return m_wicFactory.Get(); }

	auto COM_d2dFactory()		const { return m_pd2dFactory; }

	auto COM_dwFactory()		const { return m_pdwFactory; }
	auto COM_wicFactory()		const { return m_wicFactory; }

	// Direct2D Resource
protected:

	ComPtr<ID2D1Factory2>			m_pd2dFactory		{ nullptr }	;

	ComPtr<IDWriteFactory2>			m_pdwFactory		{ nullptr }	;
	ComPtr<IWICImagingFactory2>		m_wicFactory		{ nullptr }	;
};

#endif



#if USING_D3D11_INDRES && USING_D2D1_INDRES

/// <summary>
/// DirectX11�� Direct2D�� ȣȯ�Ǵ� ��ġ ������ ���ҽ��� �����մϴ�.
///
/// ��ġ �������� ���ҽ� ������ ������ �� �Լ��� �����մϴ�.
///
/// ���� ���ҽ��� �����ϰ� �����մϴ� :
///		ID3D11Device
///		ID3D11DeviceContext
///
///		ID2D1Factory
///		IDIDWriteFactory
///		IWICImagingFactory
///
///		ID2D1Device
///		ID2D1DeviceContext
/// </summary>
class CIndependentDeviceResource
	: public CDirect3DIndependentDeviceResource
	, public CDirect2DIndependentDeviceResource
{

public:

	CIndependentDeviceResource();
	~CIndependentDeviceResource();

	/// <summary>
	/// ��ġ ������ ����̽��� �����մϴ�.
	/// </summary>
	///	<param name = "bCreate2DResources"> Direct2D ���ҽ��� �������� �����մϴ�. �⺻���� true �Դϴ�. </param>
	bool Initialize() override;

	/// <summary>
	/// Direct2D Device Context �� ���� ������ Swap Chain ��ü�� Back Buffer�� ȹ���մϴ�.
	/// </summary>
	///	<param name = "pdxgiSwapChain"> Swap Chain ��ü�Դϴ�. </param>
	ComPtr<ID2D1Bitmap1> GetSwapChainBackBufferForDirect2D(IDXGISwapChain* pdxgiSwapChain) const;

	/// <summary>
	/// Texture2D ��ü�� ȣȯ�Ǵ�, Direct2D Device Context���� ��� ������ ID2D1Bitmap1 ��ü�� ��ȯ�մϴ�.
	/// </summary>
	///	<param name = "pd3dTexture2D"> Texture2D ��ü�Դϴ�. </param>
	/// <remarks> ��ȯ�� Bitmap�� Direct2D Device Context������ ����� �����մϴ�. </remarks>
	ComPtr<ID2D1Bitmap1> GetBitmapByTexture2D(ComPtr<ID3D11Texture2D> pd3dTexture2D) const;

	/// <summary>
	/// Render Target�� Back Buffer�� Texture2D ��ü�� ���� ������ Direct 2D Render Target�� ȹ���մϴ�.
	/// </summary>
	///	<param name = "pd3dTexture2D"> Texture2D ��ü�Դϴ�. </param>
	/// <remarks> 
	/// <para> RenderTarget�� Texture2D�� �����ֱ⿡ ���ӵ˴ϴ�. </para>
	/// <para> Texture2D�� ��ȿȭ�Ǹ� RenderTarget�� �ùٸ� ����� �� �� ���� ���Դϴ�. </para>
	/// </remarks>
	ComPtr<ID2D1RenderTarget> GetDirect2DRenderTargetByTexture2D(ComPtr<ID3D11Texture2D> pd3dTexture2D) const;

	// Getter
public:

	auto d2dDevice()		const { return m_pd2dDevice.Get(); }
	auto d2dDeviceContext()	const { return m_pd2dDeviceContext.Get(); }

	auto COM_d2dDevice()		const { return m_pd2dDevice; }
	auto COM_d2dDeviceContext()	const { return m_pd2dDeviceContext; }
	// Direct2D Resource
protected:

	ComPtr<ID2D1Device2>			m_pd2dDevice		{ nullptr }	;
	ComPtr<ID2D1DeviceContext2>		m_pd2dDeviceContext	{ nullptr }	;
};

#endif
