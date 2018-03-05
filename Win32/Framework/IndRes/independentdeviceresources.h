#pragma once

#include "DirectXVersion.h"

//	2017.03.26
//	설계 방향 :
//		Getter 제공
//		Initialzer은 main에서 진행
//		전역 접근이 가능해도 문제 없지 않을까 싶다. ◁ Singleton ?
//		dependent resource create 함수 제공?

#if USING_D3D11_INDRES

/// <summary>
/// DirectX11과 호환되는 장치 독립적 리소스를 생성합니다.
///
/// 장치 종속적인 리소스 생성을 간단히 할 함수를 제공합니다.
///
/// 다음 리소스를 생성하고 제공합니다 :
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
	/// 장치 독립적 디바이스를 생성합니다.
	/// </summary>
	///	<param name = "bCreate2DResources"> Direct2D 리소스를 생성할지 결정합니다. 기본값은 true 입니다. </param>
	virtual bool Initialize();

	/// <summary>
	/// Swap Chain 객체를 생성하여 반환합니다.
	/// </summary>
	///	<param name = "hWnd"> Swap Chain 객체가 출력할 윈도우 핸들입니다. </param>
	///	<param name = "dxgiFormat"> 
	/// <para> Swap Chain 객체가 사용할 DXGI_FORMAT 값입니다.</para> 
	/// <para> 이 값을 기억해야 ResizeBuffer 등의 함수에서 정확한 값을 넣을 수 있습니다.</para> 
	/// </param>
	ComPtr<IDXGISwapChain> CreateSwapChain(HWND hWnd, DXGI_FORMAT dxgiFormat) const;

	/// <summary>
	/// Swap Chain 객체를 생성하여 반환합니다.
	/// </summary>
	///	<param name = "desc"> Swap Chain 객체를 생성하는 데 쓰이는 구조체입니다. </param>
	ComPtr<IDXGISwapChain> CreateSwapChain(DXGI_SWAP_CHAIN_DESC desc) const;

	/// <summary>
	/// Swap Chain에 종속된 RenderTargetView를 생성합니다.
	/// </summary>
	///	<param name = "pdxgiSwapChain"> Swap Chain 객체입니다. </param>
	/// <remarks> 반환되는 RTV는 다른 Swap Chain 객체에서 생성된 RTV와 호환되지 않습니다. </remarks>
	ComPtr<ID3D11RenderTargetView> CreateRenderTargetViewBySwapChain(IDXGISwapChain* pdxgiSwapChain) const;

	/// <summary>
	/// Depth-Stencil Buffer를 만듭니다.
	/// </summary>
	///	<param name = "rc"> Render Target View와 동일한 크기입니다. </param>
	/// <remarks> 크기가 다르면 D3D WARNNING 이 발생합니다. </remarks>
	ComPtr<ID3D11Texture2D> CreateDepthStencilBuffer(RECT rc) const;

	/// <summary>
	/// Depth-Stencil Buffer를 만듭니다.
	/// </summary>
	///	<param name = "desc"> Buffer를 만들기 위한 Texture2D Description입니다. </param>
	/// <remarks> Render Target View와 크기가 다르면 D3D WARNNING 이 발생합니다. </remarks>
	ComPtr<ID3D11Texture2D> CreateDepthStencilBuffer(D3D11_TEXTURE2D_DESC desc) const;

	/// <summary>
	/// Depth-Stencil View를 만듭니다.
	/// </summary>
	///	<param name = "pd3dDepthStencilBuffer"> Depth-Stencil Buffer 입니다. </param>
	ComPtr<ID3D11DepthStencilView> CreateDepthStencilView(ID3D11Texture2D* pd3dDepthStencilBuffer) const;

	// Direct3D Resource
protected:

	ComPtr<ID3D11Device2>			m_pd3dDevice		{ nullptr }	;
	ComPtr<ID3D11DeviceContext2>	m_pd3dDeviceContext	{ nullptr }	;

	ComPtr<IDXGIFactory3>			m_dxgiFactory		{ nullptr }	;
	ComPtr<IDXGIDevice3>			m_dxgiDevice		{ nullptr }	;
	ComPtr<IDXGIAdapter2>			m_dxgiAdapter		{ nullptr }	;

public:

	const ComPtr<ID3D11Device2>			&	d3dDevice			{ m_pd3dDevice			 }	;
	const ComPtr<ID3D11DeviceContext2>	&	d3dDeviceContext	{ m_pd3dDeviceContext	 }	;


};

#endif



#if USING_D2D1_INDRES

/// <summary>
/// Direct2D와 호환되는 장치 독립적 리소스를 생성합니다.
///
/// 장치 종속적인 리소스 생성을 간단히 할 함수를 제공합니다.
///
/// 다음 리소스를 생성하고 제공합니다 :
///		D2D_Factory
///		IDIDWriteFactory
///		IWICImagingFactory
///
/// </summary>
class CDirect2DIndependentDeviceResource
{

public:

	CDirect2DIndependentDeviceResource();
	virtual ~CDirect2DIndependentDeviceResource();

	/// <summary>
	/// 장치 독립적 디바이스를 생성합니다.
	/// </summary>
	///	<param name = "bCreate2DResources"> Direct2D 리소스를 생성할지 결정합니다. 기본값은 true 입니다. </param>
	virtual bool Initialize();

	/// <summary>
	/// 윈도우 핸들에 종속되는 Render Target을 생성합니다.
	/// </summary>
	///	<param name = "bCreate2DResources"> Render Target이 출력할 윈도우 핸들입니다. </param>
	///	<param name = "prop"> Render Target 속성값입니다. 기본값은 RenderTargetProperties() 입니다. </param>
	///	<param name = "presentOpt"> Render Target의 출력 옵션입니다. 기본값은 D2D1_PRESENT_OPTIONS_IMMEDIATELY 입니다. </param>
	ComPtr<ID2D1HwndRenderTarget> CreateHwndRenderTarget(
		  HWND hWnd
		, D2D1_RENDER_TARGET_PROPERTIES prop = D2D1::RenderTargetProperties()
		, D2D1_PRESENT_OPTIONS presentOpt = D2D1_PRESENT_OPTIONS_IMMEDIATELY
	) const;

	// Direct2D Resource
protected:

	ComPtr<D2D_Factory>		m_pd2dFactory		{ nullptr }	;

	ComPtr<DWT_Factory>		m_pdwFactory		{ nullptr }	;
	ComPtr<WIC_Factory>		m_wicFactory		{ nullptr }	;

	bool					m_bInitializeCoInit	{ false }	;
	
public:

	const ComPtr<D2D_Factory>	&	d2dFactory		{ m_pd2dFactory	 }	;

	const ComPtr<DWT_Factory>	&	dwFactory		{ m_pdwFactory	 }	;
	const ComPtr<WIC_Factory>	&	wicFactory		{ m_wicFactory	 }	;

};

#endif



#if USING_D3D11_INDRES && USING_D2D1_INDRES

/// <summary>
/// DirectX11과 Direct2D에 호환되는 장치 독립적 리소스를 생성합니다.
///
/// 장치 종속적인 리소스 생성을 간단히 할 함수를 제공합니다.
///
/// 다음 리소스를 생성하고 제공합니다 :
///		ID3D11Device
///		ID3D11DeviceContext
///
///		D2D_Factory
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
	virtual ~CIndependentDeviceResource();

	/// <summary>
	/// 장치 독립적 디바이스를 생성합니다.
	/// </summary>
	///	<param name = "bCreate2DResources"> Direct2D 리소스를 생성할지 결정합니다. 기본값은 true 입니다. </param>
	bool Initialize() override;

	/// <summary>
	/// Direct2D Device Context 가 접근 가능한 Swap Chain 객체의 Back Buffer를 획득합니다.
	/// </summary>
	///	<param name = "pdxgiSwapChain"> Swap Chain 객체입니다. </param>
	ComPtr<ID2D1Bitmap1> GetSwapChainBackBufferForDirect2D(IDXGISwapChain* pdxgiSwapChain) const;

	/// <summary>
	/// Texture2D 객체와 호환되는, Direct2D Device Context에서 사용 가능한 ID2D1Bitmap1 객체를 반환합니다.
	/// </summary>
	///	<param name = "pd3dTexture2D"> Texture2D 객체입니다. </param>
	/// <remarks> 반환된 Bitmap은 Direct2D Device Context에서만 사용이 가능합니다. </remarks>
	ComPtr<ID2D1Bitmap1> GetBitmapByTexture2D(ComPtr<ID3D11Texture2D> pd3dTexture2D) const;

	/// <summary>
	/// Render Target의 Back Buffer인 Texture2D 객체에 접근 가능한 Direct 2D Render Target을 획득합니다.
	/// </summary>
	///	<param name = "pd3dTexture2D"> Texture2D 객체입니다. </param>
	/// <remarks> 
	/// <para> RenderTarget는 Texture2D의 생명주기에 종속됩니다. </para>
	/// <para> Texture2D가 무효화되면 RenderTarget은 올바른 출력을 할 수 없을 것입니다. </para>
	/// </remarks>
	ComPtr<ID2D1RenderTarget> GetDirect2DRenderTargetByTexture2D(ComPtr<ID3D11Texture2D> pd3dTexture2D) const;

	// Direct2D Resource
protected:

	ComPtr<ID2D1Device1>			m_pd2dDevice		{ nullptr }	;
	ComPtr<ID2D1DeviceContext>		m_pd2dDeviceContext	{ nullptr }	;
	
public:
	
	const ComPtr<ID2D1Device1>			&	d2dDevice			{ m_pd2dDevice			}	;
	const ComPtr<ID2D1DeviceContext>	&	d2dDeviceContext	{ m_pd2dDeviceContext	}	;
	
};

#endif
