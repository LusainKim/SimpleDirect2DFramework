#include "stdafx.h"
#include "IndependentDeviceResources.h"

#if USING_D3D11_INDRES

CDirect3DIndependentDeviceResource::CDirect3DIndependentDeviceResource()
{
}

CDirect3DIndependentDeviceResource::~CDirect3DIndependentDeviceResource()
{
}

bool CDirect3DIndependentDeviceResource::Initialize()
{
	// �����ϴ� �ϵ���� �׷��� ����̹��� �����մϴ�.
	D3D_DRIVER_TYPE d3dDriverTypes[]
	{
		// �ϵ���� ����̹�, Direct3D�� ����� �ִ��� �ϵ����� ���� 
		// �ϵ���������� �������� �ʴ� ����� ����Ʈ��� ���� ����
		  D3D_DRIVER_TYPE_HARDWARE
		// DirectX SDK���� �����ϴ� ������ ����Ʈ���� ����̹�
		// Ư�� ���� 9_1���� Ư�� ���� 10.1���� ����
		// - ������ ����Ʈ���� ����̹�(�����Ͷ�����)�� �ʿ����
		// - ����Ʈ���� ����̹��̸鼭 �׷��� �ϵ������ �ִ� ������ ����
		//   Direct3D 11�� �������� �ʴ� �׷��� �ϵ������� ���
		, D3D_DRIVER_TYPE_WARP
		// ���� ����̹�(��� Direct3D Ư���� �����ϴ� ����Ʈ���� ����̹�)
		// DirectX SDK���� �ӵ����ٴ� ��Ȯ���� ���� ����
		// CPU�� ����Ͽ� �����Ͷ����� ���� 
		// Direct3D 11�� ��� Ư�� �������� �� ����̹��� ����� �� ����
		// ���� ���α׷��� �׽�Ʈ�� ������� ���� �ַ� ���
		, D3D_DRIVER_TYPE_REFERENCE
	};

	// �� �迭�� �� ���� ���α׷����� �����ϴ� DirectX �ϵ���� ��� ���� ������ �����մϴ�.
	// ������ �����ؾ� �մϴ�.
	// ������ ���� ���α׷��� �ʿ��� �ּ� ��� ������ �����ؾ� �մϴ�.
	// ������ �������� ���� ��� ��� ���� ���α׷��� 9.1�� �����ϴ� ������ ���ֵ˴ϴ�.
	D3D_FEATURE_LEVEL d3dFeatureLevels[]
	{
		  D3D_FEATURE_LEVEL_11_1
		, D3D_FEATURE_LEVEL_11_0
		, D3D_FEATURE_LEVEL_10_1
		, D3D_FEATURE_LEVEL_10_0
	};
	// ���� ������ ��� �ϵ���� ��� ���� ���� ���� ���մϴ�.
	auto szFeatureLevel = static_cast<UINT>(ARRAYSIZE(d3dFeatureLevels));

	// ���������� ���� ��ġ�� ��� ������ �����ϱ� ���� �����Դϴ�.
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	// Result Handle �Դϴ�. ��ġ�� ���������� �����Ǵ��� �˻��մϴ�.
	HRESULT hResult;

	// �� �÷��״� API �⺻���� �ٸ� �� ä�� ������ ǥ�鿡 ���� ������
	// �߰��մϴ�. Direct2D���� ȣȯ���� ���� �ʿ��մϴ�.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	
#if defined(_DEBUG)

	auto IsSDKLayerAvailable = []() -> bool {
		return SUCCEEDED(D3D11CreateDevice(
			  nullptr
			, D3D_DRIVER_TYPE_NULL			// ���� �ϵ���� ��ġ�� ���� �ʿ䰡 �����ϴ�.
			, 0
			, D3D11_CREATE_DEVICE_DEBUG		// SDK ���̾ Ȯ���ϼ���.
			, nullptr						// ��� ��� ������ ����˴ϴ�.
			, 0
			, D3D11_SDK_VERSION				// Windows ����� ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
			, nullptr						// D3D ��ġ ������ ������ �ʿ䰡 �����ϴ�.
			, nullptr						// ��� ������ �� �ʿ䰡 �����ϴ�.
			, nullptr						// D3D ��ġ ���ؽ�Ʈ ������ ������ �ʿ䰡 �����ϴ�.
		));
	};

	// SDK ���̾� ������ Ȯ���ϼ���.
	if (IsSDKLayerAvailable())
	{
		// ������Ʈ�� ����� ���� ���� ��쿡�� �� �÷��װ� �ִ� SDK ���̾ ���� ������� ����Ͻʽÿ�.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

#endif

#if USE_BEST_GPU
	ComPtr<IDXGIFactory2> dxgiFactory;
	UINT dxgiFlag = 0;
#ifdef _DEBUG
	dxgiFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	if (FAILED(hResult = CreateDXGIFactory2(dxgiFlag, __uuidof(IDXGIFactory2), &dxgiFactory)))
	{
		MessageBox(nullptr, TEXT("DXGI Factory ��ü�� ��ȯ���� ���߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}
	dxgiFactory.As(&m_dxgiFactory);

	UINT adapterIdx = 0;
	size_t szBestVideoMemory = 0;
	while (true)
	{
		DXGI_ADAPTER_DESC1 desc;
		ComPtr<IDXGIAdapter1> dxgiAdapter;
		if (m_dxgiFactory->EnumAdapters1(adapterIdx, &dxgiAdapter) == DXGI_ERROR_NOT_FOUND)
			break;
		dxgiAdapter->GetDesc1(&desc);
		size_t szVideoMemory = static_cast<size_t>(desc.DedicatedVideoMemory + desc.SharedSystemMemory);

		if (szBestVideoMemory < szVideoMemory)
		{
			szBestVideoMemory = szVideoMemory;
			dxgiAdapter.As(&m_dxgiAdapter);
			// Debug view
			wstring strDebug = desc.Description + L", "s + FixValue(szBestVideoMemory / 1024 / 1024) + L" MB"s;
		//	MessageBox(nullptr, strDebug.c_str(), L"GPU Info", MB_OK);
		}
		++adapterIdx;
	}
#endif

	// Direct3D 11 API ��ġ ��ü�� �ش� ���ؽ�Ʈ�� ����ϴ�.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	if (m_dxgiAdapter)
	{
		hResult = D3D11CreateDevice(
				  m_dxgiAdapter.Get()		// �⺻ ����͸� ����Ϸ��� nullptr�� �����մϴ�.
				, D3D_DRIVER_TYPE_UNKNOWN	// �ϵ���� �׷��� ����̹��� ����Ͽ� ��ġ�� ����ϴ�.
				, nullptr					// ����̹��� D3D_DRIVER_TYPE_SOFTWARE�� �ƴ� ��� 0�̾�� �մϴ�.
				, creationFlags				// ����� �� Direct2D ȣȯ�� �÷��׸� �����մϴ�.
				, d3dFeatureLevels			// �� ���� ���α׷��� ������ �� �ִ� ��� ���� ����Դϴ�.
				, szFeatureLevel			// �� ����� ũ���Դϴ�.
				, D3D11_SDK_VERSION			// Windows ����� ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
				, &device					// ������� Direct3D ��ġ�� ��ȯ�մϴ�.
				, &nd3dFeatureLevel			// ������� ��ġ�� ��� ������ ��ȯ�մϴ�.
				, &context					// ��ġ ���� ���ؽ�Ʈ�� ��ȯ�մϴ�.
			);

		if (SUCCEEDED(hResult))
		{
			device .As(&m_pd3dDevice);
			context.As(&m_pd3dDeviceContext);
		}
	}
	else
	{
		for (D3D_DRIVER_TYPE &idxDreiverType : d3dDriverTypes)
		{
			hResult = D3D11CreateDevice(
				  nullptr					// �⺻ ����͸� ����Ϸ��� nullptr�� �����մϴ�.
				, idxDreiverType			// �ϵ���� �׷��� ����̹��� ����Ͽ� ��ġ�� ����ϴ�.
				, nullptr					// ����̹��� D3D_DRIVER_TYPE_SOFTWARE�� �ƴ� ��� 0�̾�� �մϴ�.
				, creationFlags				// ����� �� Direct2D ȣȯ�� �÷��׸� �����մϴ�.
				, d3dFeatureLevels			// �� ���� ���α׷��� ������ �� �ִ� ��� ���� ����Դϴ�.
				, szFeatureLevel			// �� ����� ũ���Դϴ�.
				, D3D11_SDK_VERSION			// Windows ����� ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
				, &device					// ������� Direct3D ��ġ�� ��ȯ�մϴ�.
				, &nd3dFeatureLevel			// ������� ��ġ�� ��� ������ ��ȯ�մϴ�.
				, &context					// ��ġ ���� ���ؽ�Ʈ�� ��ȯ�մϴ�.
			);

			if (SUCCEEDED(hResult))
			{
				device .As(&m_pd3dDevice);
				context.As(&m_pd3dDeviceContext);

				break;
			}
		}
	}

	// ��� ���� ������ ����̹����� ������ �����ϸ� ���α׷��� �����մϴ�.
	if (!m_pd3dDevice)
	{
		MessageBox(nullptr, TEXT("���� ������ �׷��� ����� �����ϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}

	// DXGI Device �� �޽��ϴ�.
	if (FAILED(hResult = m_pd3dDevice.As(&m_dxgiDevice)))
	{
		MessageBox(nullptr, TEXT("DXGI Device ��ü�� ��ȯ���� ���߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}

	if (!m_dxgiAdapter || !m_dxgiFactory)
	{
		// DXGI Device ���� Adapter�� �޽��ϴ�. DXGI Factory�� �����ϱ� ���� �ʿ��մϴ�.
		ComPtr<IDXGIAdapter> pdxgiAdapter;
		if (FAILED(hResult = m_dxgiDevice->GetAdapter(&pdxgiAdapter)))
		{
			MessageBox(nullptr, TEXT("DXGIAdapter������ ��ü ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}
		pdxgiAdapter.As(&m_dxgiAdapter);

		// View Description
		DXGI_ADAPTER_DESC2 desc;
		m_dxgiAdapter->GetDesc2(&desc);

		// Debug view MessageBox;
		wstring strDebug = desc.Description + L", "s + FixValue((desc.DedicatedVideoMemory + desc.SharedSystemMemory) / 1024 / 1024) + L" MB"s;
//		MessageBox(nullptr, strDebug.c_str(), L"GPU Info", MB_OK);

		// DXGI Factory �ν��Ͻ��� DXGIFactory���� �޽��ϴ�.
		if (FAILED(hResult = m_dxgiAdapter->GetParent(IID_PPV_ARGS(&m_dxgiFactory))))
		{
			MessageBox(nullptr, TEXT("DXGIFactory������ ��ü ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}
	}

	return true;
}

ComPtr<IDXGISwapChain> CDirect3DIndependentDeviceResource::CreateSwapChain(HWND hWnd, DXGI_FORMAT dxgiFormat) const
{
	// Swap Chain Description ����ü
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;

	RECT rcClient{};
	::GetClientRect(hWnd, &rcClient);

	#pragma region [DXGI_SWAP_CHAIN_DESC �ʱ�ȭ]
	// ����ü ����
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	// BufferCount : �ĸ� ������ ���� ����
	dxgiSwapChainDesc.BufferCount = 2;
		
	// BufferDesc : �ĸ� ������ ���÷��� ������ ����
	{
		// Width : ������ ���� ũ�⸦ �ȼ��� ����
		dxgiSwapChainDesc.BufferDesc.Width = rcClient.right;
		// Height : ������ ���� ũ�⸦ �ȼ��� ����
		dxgiSwapChainDesc.BufferDesc.Height = rcClient.bottom;
		// Format : �ĸ� ���� �ȼ� ����
		/// DirectX 11-1(Chap 01)-Device, p.49 ����
		dxgiSwapChainDesc.BufferDesc.Format = dxgiFormat;
		// RefreshRate : ȭ�� ���� ������ Hz ������ ����
		{
			// Denominator : �и�
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			// Numerator : ����
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		}
		// ScanlineOrdering : scan line �׸��� ��� ����(�⺻ 0)
		//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED        (0) : ��ĵ ���� ������ �������� ����	
		//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE        (1) : ���α׷��ú� ��ĵ ���� ���� ����
		//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST  (2) : ���� �ʵ�� �̹��� ����
		//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST  (3) : ���� �ʵ�� �̹��� ����
		dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}

	// BufferUsage : �ĸ� ���ۿ� ���� ǥ�� ��� ��İ� CPU�� ���� ��� ����
	//	DXGI_USAGE_SHADER_INPUT				: shader �� �Է����� ���
	//	DXGI_USAGE_RENDER_TARGET_OUTPUT		: render target���� ���
	//	DXGI_USAGE_BACK_BUFFER         		: back buffer �� ���
	//	DXGI_USAGE_SHARED              		: ���� ����
	//	DXGI_USAGE_READ_ONLY           		: �б� ����
	//	DXGI_USAGE_DISCARD_ON_PRESENT  		: DXGI ���� ���� ���(����ڰ� ������� ����)
	//	DXGI_USAGE_UNORDERED_ACCESS    		: ������ȭ ����
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// OutputWindow : ��� �������� �ڵ��� ����(�ݵ�� ��ȿ�ؾ� ��)
	dxgiSwapChainDesc.OutputWindow = hWnd;

	// SampleDesc : ���� ���ø��� ǰ���� ����
	// CheckMultisampleQualityLevels �Լ��� ����Ͽ� ���� ���ø� ���� ���θ� Ȯ���� �ڿ� �� ����
	{
		// Count : �ȼ� �� ���� ����
		//	1  : ���� ���ø��� ���� ����
		//	2~ : �ش� ����ŭ�� ���� ���ø�
		dxgiSwapChainDesc.SampleDesc.Count = 1;
		// Quality : ǰ�� ����
		// 0 : ���� ���ø��� ���� ����
		dxgiSwapChainDesc.SampleDesc.Quality = 0;
	}

	// Windowed : ������ ��� �Ǵ� ��ü ȭ�� ��� ����	~ TRUE  | â ��� 
	//												~ FALSE | ��ü ȭ��
	dxgiSwapChainDesc.Windowed = TRUE;

	// Flags : Swap Chain ���ۿ� ���� ���� ������ ����
	//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		(1) : ��ü ȭ�� ��忡�� ���� ������ ������ ȭ������ �ű� �� �ڵ� ȸ������ ����
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	(2) : ���� ���α׷��� ���÷��� ��带 ������ �� ����
	//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		(4) : ���� ���α׷��� GDI �� ������ �� �� ����. �ĸ� ���ۿ� GetDC() ��� ����
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;

	// SwapEffect : Swaping�� ó���ϴ� ���û����� ����(�⺻ : 0)
	//	DXGI_SWAP_EFFECT_DISCARD		(0) : ���� ������ ���
	//	DXGI_SWAP_EFFECT_SEQUENTIAL		(1) : ���� ����
	// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	(2) : Flip ���� ����
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	
	#pragma endregion

	return CreateSwapChain(dxgiSwapChainDesc);
}

ComPtr<IDXGISwapChain> CDirect3DIndependentDeviceResource::CreateSwapChain(DXGI_SWAP_CHAIN_DESC desc) const
{
	ComPtr<IDXGISwapChain> pdxgiSwapChain;
	if (FAILED(m_dxgiFactory->CreateSwapChain(m_dxgiDevice.Get(), &desc, &pdxgiSwapChain)))
	{
		MessageBox(nullptr, TEXT("SwapChain �ν��Ͻ� ������ �����߽��ϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return ComPtr<IDXGISwapChain>();
	}

	return pdxgiSwapChain;
}

ComPtr<ID3D11RenderTargetView> CDirect3DIndependentDeviceResource::CreateRenderTargetViewBySwapChain(IDXGISwapChain * pdxgiSwapChain) const
{
	// ��ȯ�Ǵ� ���Դϴ�.
	ComPtr<ID3D11RenderTargetView> rtv{ nullptr };

	// Result Handle �Դϴ�. ��ġ�� ���������� �����Ǵ��� �˻��մϴ�.
	HRESULT hResult; 
		
	// render target �� �����ϱ� ���� back buffer �� SwapChain ���� ��û�մϴ�.
	ComPtr<ID3D11Texture2D> pd3dBackBuffer{ nullptr };

	hResult = pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<LPVOID*>(&pd3dBackBuffer));
	if (FAILED(hResult)) return rtv;

	// ��ȯ���� ���۸� ����Ͽ� render target view �� �����մϴ�.
	m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer.Get(), nullptr, &rtv);

	return rtv;
}

ComPtr<ID3D11Texture2D> CDirect3DIndependentDeviceResource::CreateDepthStencilBuffer(RECT rcClient) const
{
	#pragma region [Create Depth-Stencil Buffer]

	// depth stencil "texture" �� �����մϴ�.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	
	// �޸𸮴� 0���� �ʱ�ȭ�մϴ�.
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Width : texture �� �ʺ��Դϴ�.
	d3dDepthStencilBufferDesc.Width = rcClient.right;
	// Height : texture �� �����Դϴ�.
	d3dDepthStencilBufferDesc.Height = rcClient.bottom;
	// MipLevels : texture �ִ� MipMap Level ��. 
	//				���� ���ø� �ؽ�ó : 1
	//				�ִ� �Ӹ� ���� : 0
	d3dDepthStencilBufferDesc.MipLevels = 1;
	// ArraySize :texture �迭�� texture ����. (�迭�� �ƴϸ� 1)
	d3dDepthStencilBufferDesc.ArraySize = 1;
	// Format : texture �ȼ� ����
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// SampleDesc : ���� ���ø��� ǰ���� ����
	// CheckMultisampleQualityLevels �Լ��� ����Ͽ� ���� ���ø� ���� ���θ� Ȯ���� �ڿ� �� ����
	{
		// Count : �ȼ� �� ���� ����
		//	1  : ���� ���ø��� ���� ����
		//	2~ : �ش� ����ŭ�� ���� ���ø�
		d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
		// Quality : ǰ�� ����
		// 0 : ���� ���ø��� ���� ����
		d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	}

	// Usage : texture �� �а� ���� ����� ���� ����
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// BindFlags : ���������� �ܰ� ��� ������ ������ ����
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	// MiscFlags : ���ҽ��� ���� �߰� ���� ����. ������� ������ 0.
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	// CPUAccessFlags : CPU�� ���۸� ����� �� �ִ� ����. ������� ������ 0.
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;

	#pragma endregion

	return CreateDepthStencilBuffer(d3dDepthStencilBufferDesc);
}

ComPtr<ID3D11Texture2D> CDirect3DIndependentDeviceResource::CreateDepthStencilBuffer(D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc) const
{
	// ��ȯ�� ���Դϴ�.
	ComPtr<ID3D11Texture2D> txt2D;

	m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, nullptr, &txt2D);
	
	return txt2D;
}

ComPtr<ID3D11DepthStencilView> CDirect3DIndependentDeviceResource::CreateDepthStencilView(ID3D11Texture2D * pd3dDepthStencilBuffer) const
{
	// ��ȯ�� ���Դϴ�.
	ComPtr<ID3D11DepthStencilView> dsv;

	// Depth-Stencil Buffer�� Description�� ȹ���մϴ�.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	pd3dDepthStencilBuffer->GetDesc(&d3dDepthStencilBufferDesc);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;

	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

	m_pd3dDevice->CreateDepthStencilView(pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &dsv);

	return dsv;
}

#endif


#if USING_D2D1_INDRES

CDirect2DIndependentDeviceResource::CDirect2DIndependentDeviceResource()
{
}

CDirect2DIndependentDeviceResource::~CDirect2DIndependentDeviceResource()
{
	if (m_bInitializeCoInit)
	{
		m_wicFactory = nullptr;
		::CoUninitialize();
	}
}

bool CDirect2DIndependentDeviceResource::Initialize()
{
	HRESULT hResult;

	// Direct2D ���ҽ��� �ʱ�ȭ�մϴ�.
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)

	// ������Ʈ�� ����� ���� ���� ��� SDK ���̾ ���� Direct2D ������� ����մϴ�.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

#endif

	// Direct2D ���͸��� �ʱ�ȭ�մϴ�.
	if(FAILED(hResult = D2D1CreateFactory(	  D2D1_FACTORY_TYPE_MULTI_THREADED
											, __uuidof(decltype(m_pd2dFactory)::InterfaceType)
											, &options
											, &m_pd2dFactory
	))) goto ReleaseDXGI;

	// DirectWrite ���͸��� �ʱ�ȭ�մϴ�.
	if (FAILED(hResult = DWriteCreateFactory(	  DWRITE_FACTORY_TYPE_SHARED
												, __uuidof(decltype(m_pdwFactory)::InterfaceType)
												, &m_pdwFactory
	))) goto ReleaseDXGI;
	
	// COM Library�� �ʱ�ȭ�մϴ�.
	// UWP���� ��� �� ���ư����� ���⼱ �� ���ư���.
	if (FAILED(hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) goto ReleaseDXGI;
	m_bInitializeCoInit = true;

	// WIC(Windows Imaging Container) ���͸��� �ʱ�ȭ�մϴ�.
	if (FAILED(hResult = CoCreateInstance(	  CLSID_WICImagingFactory
											, nullptr
											, CLSCTX_INPROC_SERVER
											, IID_PPV_ARGS(&m_wicFactory)
	))) goto ReleaseDXGI;

ReleaseDXGI:
	return (!FAILED(hResult));

}

ComPtr<ID2D1HwndRenderTarget> CDirect2DIndependentDeviceResource::CreateHwndRenderTarget(HWND hWnd, D2D1_RENDER_TARGET_PROPERTIES prop, D2D1_PRESENT_OPTIONS presentOpt) const
{
	ComPtr<ID2D1HwndRenderTarget> phwndRenderTarget{ nullptr };
	
	RECT rc{};
	::GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size
	{
		  static_cast<UINT32>(rc.right - rc.left)
		, static_cast<UINT32>(rc.bottom - rc.top)
	};

	m_pd2dFactory->CreateHwndRenderTarget
	(
		  prop
		, D2D1::HwndRenderTargetProperties(hWnd, size, presentOpt)
		, &phwndRenderTarget
	);

	return phwndRenderTarget;
}

#endif


#if USING_D3D11_INDRES && USING_D2D1_INDRES

CIndependentDeviceResource::CIndependentDeviceResource()
{
}

CIndependentDeviceResource::~CIndependentDeviceResource()
{
}

bool CIndependentDeviceResource::Initialize()
{
	bool retval	=	CDirect3DIndependentDeviceResource::Initialize()
				&&	CDirect2DIndependentDeviceResource::Initialize();
	if (!retval) return false;
	
	// Direct3D�� Direct2D�� ȣȯ�� ���� �Լ� ����
	m_pd2dFactory->CreateDevice(m_dxgiDevice.Get(), &m_pd2dDevice);

	// Direct2D DC�� �����մϴ�.
	m_pd2dDevice->CreateDeviceContext(
		  D2D1_DEVICE_CONTEXT_OPTIONS_NONE
		, &m_pd2dDeviceContext
	);
	
	return true;
}

ComPtr<ID2D1Bitmap1> CIndependentDeviceResource::GetBitmapByTexture2D(ComPtr<ID3D11Texture2D> pd3dTexture2D) const
{
	ComPtr<ID2D1Bitmap1> bitmap;
	
	ComPtr<IDXGISurface> pDxgiSurface;
	pd3dTexture2D.As(&pDxgiSurface);
	m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(	  pDxgiSurface.Get()
														, nullptr
														, &bitmap);
	return bitmap;
}

ComPtr<ID2D1RenderTarget> CIndependentDeviceResource::GetDirect2DRenderTargetByTexture2D(ComPtr<ID3D11Texture2D> pd3dTexture2D) const
{
	ComPtr<ID2D1RenderTarget> pd2dRenderTarget{ nullptr };

	ComPtr<IDXGISurface> pDxgiSurface;
	pd3dTexture2D.As(&pDxgiSurface);

	FLOAT dpiX;
	FLOAT dpiY;

	m_pd2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			  D2D1_RENDER_TARGET_TYPE_DEFAULT
			, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
			, dpiX
			, dpiY
	);

	m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pDxgiSurface.Get(), props, &pd2dRenderTarget);

	return pd2dRenderTarget;
}

ComPtr<ID2D1Bitmap1> CIndependentDeviceResource::GetSwapChainBackBufferForDirect2D(IDXGISwapChain* pdxgiSwapChain) const
{
	ComPtr<ID2D1Bitmap1> pd2dBmpBackBuffer{nullptr};
	// ȭ���� �ػ󵵸� ����ϴ�.
	float fdpiX, fdpiY;
	m_pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
	
	// dxgi format�� �˱� ���� Swap Chain ��ü�� Description�� ȹ���մϴ�. 
	DXGI_SWAP_CHAIN_DESC desc;
	pdxgiSwapChain->GetDesc(&desc);
	
	HRESULT hResult;

	// ���� ü�� �� ���ۿ� ����� Direct2D ���
	// ��Ʈ���� ����� �̸� ���� ������� �����մϴ�.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		BitmapProperties1(	  D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW
							, PixelFormat(desc.BufferDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED)
							, fdpiX
							, fdpiY
		);

	// DXGI ǥ���� ����ϴ�.
	ComPtr<IDXGISurface2> dxgiBackBuffer;
	if (FAILED(hResult = pdxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)
	))) return pd2dBmpBackBuffer;

	m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(	  dxgiBackBuffer.Get()
														, &bitmapProperties
														, &pd2dBmpBackBuffer
	);

//		m_pd2dDeviceContext->SetTarget(m_pd2dBmpBackBuffer.Get());
//		m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
//
//		return true;

	return pd2dBmpBackBuffer;
}

#endif
