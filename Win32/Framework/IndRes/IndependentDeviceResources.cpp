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
	// 지원하는 하드웨어 그래픽 드라이버를 열거합니다.
	D3D_DRIVER_TYPE d3dDriverTypes[]
	{
		// 하드웨어 드라이버, Direct3D의 기능이 최대한 하드웨어로 구현 
		// 하드웨어적으로 제공하지 않는 기능은 소프트웨어를 통해 구현
		  D3D_DRIVER_TYPE_HARDWARE
		// DirectX SDK에서 제공하는 고성능의 소프트웨어 드라이버
		// 특성 레벨 9_1에서 특성 레벨 10.1까지 제공
		// - 별도의 소프트웨어 드라이버(래스터라이저)가 필요없음
		// - 소프트웨어 드라이버이면서 그래픽 하드웨어의 최대 성능을 지원
		//   Direct3D 11을 지원하지 않는 그래픽 하드웨어에서도 사용
		, D3D_DRIVER_TYPE_WARP
		// 참조 드라이버(모든 Direct3D 특성을 지원하는 소프트웨어 드라이버)
		// DirectX SDK에서 속도보다는 정확성을 위해 제공
		// CPU를 사용하여 래스터라이저 구현 
		// Direct3D 11의 모든 특성 레벨에서 이 드라이버를 사용할 수 있음
		// 응용 프로그램의 테스트와 디버깅을 위해 주로 사용
		, D3D_DRIVER_TYPE_REFERENCE
	};

	// 이 배열은 이 응용 프로그램에서 지원하는 DirectX 하드웨어 기능 수준 집합을 정의합니다.
	// 순서를 유지해야 합니다.
	// 설명에서 응용 프로그램에 필요한 최소 기능 수준을 선언해야 합니다.
	// 별도로 지정하지 않은 경우 모든 응용 프로그램은 9.1을 지원하는 것으로 간주됩니다.
	D3D_FEATURE_LEVEL d3dFeatureLevels[]
	{
		  D3D_FEATURE_LEVEL_11_1
		, D3D_FEATURE_LEVEL_11_0
		, D3D_FEATURE_LEVEL_10_1
		, D3D_FEATURE_LEVEL_10_0
	};
	// 지원 가능한 모든 하드웨어 기능 수준 집합 수를 구합니다.
	auto szFeatureLevel = static_cast<UINT>(ARRAYSIZE(d3dFeatureLevels));

	// 최종적으로 사용될 장치의 기능 수준을 저장하기 위한 변수입니다.
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	// Result Handle 입니다. 장치가 성공적으로 생성되는지 검사합니다.
	HRESULT hResult;

	// 이 플래그는 API 기본값과 다른 색 채널 순서의 표면에 대한 지원을
	// 추가합니다. Direct2D와의 호환성을 위해 필요합니다.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	
#if defined(_DEBUG)

	auto IsSDKLayerAvailable = []() -> bool {
		return SUCCEEDED(D3D11CreateDevice(
			  nullptr
			, D3D_DRIVER_TYPE_NULL			// 실제 하드웨어 장치를 만들 필요가 없습니다.
			, 0
			, D3D11_CREATE_DEVICE_DEBUG		// SDK 레이어를 확인하세요.
			, nullptr						// 모든 기능 수준이 적용됩니다.
			, 0
			, D3D11_SDK_VERSION				// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
			, nullptr						// D3D 장치 참조를 보관할 필요가 없습니다.
			, nullptr						// 기능 수준을 알 필요가 없습니다.
			, nullptr						// D3D 장치 컨텍스트 참조를 보관할 필요가 없습니다.
		));
	};

	// SDK 레이어 지원을 확인하세요.
	if (IsSDKLayerAvailable())
	{
		// 프로젝트가 디버그 빌드 중인 경우에는 이 플래그가 있는 SDK 레이어를 통해 디버깅을 사용하십시오.
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
		MessageBox(nullptr, TEXT("DXGI Factory 객체를 반환받지 못했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
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

	// Direct3D 11 API 장치 개체와 해당 컨텍스트를 만듭니다.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	if (m_dxgiAdapter)
	{
		hResult = D3D11CreateDevice(
				  m_dxgiAdapter.Get()		// 기본 어댑터를 사용하려면 nullptr을 지정합니다.
				, D3D_DRIVER_TYPE_UNKNOWN	// 하드웨어 그래픽 드라이버를 사용하여 장치를 만듭니다.
				, nullptr					// 드라이버가 D3D_DRIVER_TYPE_SOFTWARE가 아닌 경우 0이어야 합니다.
				, creationFlags				// 디버그 및 Direct2D 호환성 플래그를 설정합니다.
				, d3dFeatureLevels			// 이 응용 프로그램이 지원할 수 있는 기능 수준 목록입니다.
				, szFeatureLevel			// 위 목록의 크기입니다.
				, D3D11_SDK_VERSION			// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
				, &device					// 만들어진 Direct3D 장치를 반환합니다.
				, &nd3dFeatureLevel			// 만들어진 장치의 기능 수준을 반환합니다.
				, &context					// 장치 직접 컨텍스트를 반환합니다.
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
				  nullptr					// 기본 어댑터를 사용하려면 nullptr을 지정합니다.
				, idxDreiverType			// 하드웨어 그래픽 드라이버를 사용하여 장치를 만듭니다.
				, nullptr					// 드라이버가 D3D_DRIVER_TYPE_SOFTWARE가 아닌 경우 0이어야 합니다.
				, creationFlags				// 디버그 및 Direct2D 호환성 플래그를 설정합니다.
				, d3dFeatureLevels			// 이 응용 프로그램이 지원할 수 있는 기능 수준 목록입니다.
				, szFeatureLevel			// 위 목록의 크기입니다.
				, D3D11_SDK_VERSION			// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
				, &device					// 만들어진 Direct3D 장치를 반환합니다.
				, &nd3dFeatureLevel			// 만들어진 장치의 기능 수준을 반환합니다.
				, &context					// 장치 직접 컨텍스트를 반환합니다.
			);

			if (SUCCEEDED(hResult))
			{
				device .As(&m_pd3dDevice);
				context.As(&m_pd3dDeviceContext);

				break;
			}
		}
	}

	// 모든 지원 가능한 드라이버에서 생성이 실패하면 프로그램을 종료합니다.
	if (!m_pd3dDevice)
	{
		MessageBox(nullptr, TEXT("지원 가능한 그래픽 사양이 없습니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}

	// DXGI Device 를 받습니다.
	if (FAILED(hResult = m_pd3dDevice.As(&m_dxgiDevice)))
	{
		MessageBox(nullptr, TEXT("DXGI Device 객체를 반환받지 못했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}

	if (!m_dxgiAdapter || !m_dxgiFactory)
	{
		// DXGI Device 에서 Adapter를 받습니다. DXGI Factory를 생성하기 위해 필요합니다.
		ComPtr<IDXGIAdapter> pdxgiAdapter;
		if (FAILED(hResult = m_dxgiDevice->GetAdapter(&pdxgiAdapter)))
		{
			MessageBox(nullptr, TEXT("DXGIAdapter에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}
		pdxgiAdapter.As(&m_dxgiAdapter);

		// View Description
		DXGI_ADAPTER_DESC2 desc;
		m_dxgiAdapter->GetDesc2(&desc);

		// Debug view MessageBox;
		wstring strDebug = desc.Description + L", "s + FixValue((desc.DedicatedVideoMemory + desc.SharedSystemMemory) / 1024 / 1024) + L" MB"s;
//		MessageBox(nullptr, strDebug.c_str(), L"GPU Info", MB_OK);

		// DXGI Factory 인스턴스를 DXGIFactory에서 받습니다.
		if (FAILED(hResult = m_dxgiAdapter->GetParent(IID_PPV_ARGS(&m_dxgiFactory))))
		{
			MessageBox(nullptr, TEXT("DXGIFactory에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}
	}

	return true;
}

ComPtr<IDXGISwapChain> CDirect3DIndependentDeviceResource::CreateSwapChain(HWND hWnd, DXGI_FORMAT dxgiFormat) const
{
	// Swap Chain Description 구조체
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;

	RECT rcClient{};
	::GetClientRect(hWnd, &rcClient);

	#pragma region [DXGI_SWAP_CHAIN_DESC 초기화]
	// 구조체 비우기
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	// BufferCount : 후면 버퍼의 수를 지정
	dxgiSwapChainDesc.BufferCount = 2;
		
	// BufferDesc : 후면 버퍼의 디스플레이 형식을 지정
	{
		// Width : 버퍼의 가로 크기를 픽셀로 지정
		dxgiSwapChainDesc.BufferDesc.Width = rcClient.right;
		// Height : 버퍼의 세로 크기를 픽셀로 지정
		dxgiSwapChainDesc.BufferDesc.Height = rcClient.bottom;
		// Format : 후면 버퍼 픽셀 형식
		/// DirectX 11-1(Chap 01)-Device, p.49 참조
		dxgiSwapChainDesc.BufferDesc.Format = dxgiFormat;
		// RefreshRate : 화면 갱신 비율을 Hz 단위로 지정
		{
			// Denominator : 분모
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			// Numerator : 분자
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		}
		// ScanlineOrdering : scan line 그리기 모드 지정(기본 0)
		//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED        (0) : 스캔 라인 순서를 지정하지 않음	
		//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE        (1) : 프로그레시브 스캔 라인 순서 지정
		//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST  (2) : 상위 필드로 이미지 생성
		//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST  (3) : 하위 필드로 이미지 생성
		dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}

	// BufferUsage : 후면 버퍼에 대한 표면 사용 방식과 CPU의 접근 방법 지정
	//	DXGI_USAGE_SHADER_INPUT				: shader 의 입력으로 사용
	//	DXGI_USAGE_RENDER_TARGET_OUTPUT		: render target으로 사용
	//	DXGI_USAGE_BACK_BUFFER         		: back buffer 로 사용
	//	DXGI_USAGE_SHARED              		: 공유 목적
	//	DXGI_USAGE_READ_ONLY           		: 읽기 전용
	//	DXGI_USAGE_DISCARD_ON_PRESENT  		: DXGI 내부 전용 사용(사용자가 사용하지 않음)
	//	DXGI_USAGE_UNORDERED_ACCESS    		: 무순서화 접근
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// OutputWindow : 출력 윈도우의 핸들을 지정(반드시 유효해야 함)
	dxgiSwapChainDesc.OutputWindow = hWnd;

	// SampleDesc : 다중 샘플링의 품질을 지정
	// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
	{
		// Count : 픽셀 당 샘플 개수
		//	1  : 다중 샘플링을 하지 않음
		//	2~ : 해당 수만큼의 다중 샘플링
		dxgiSwapChainDesc.SampleDesc.Count = 1;
		// Quality : 품질 레벨
		// 0 : 다중 샘플링을 하지 않음
		dxgiSwapChainDesc.SampleDesc.Quality = 0;
	}

	// Windowed : 윈도우 모드 또는 전체 화면 모드 지정	~ TRUE  | 창 모드 
	//												~ FALSE | 전체 화면
	dxgiSwapChainDesc.Windowed = TRUE;

	// Flags : Swap Chain 동작에 대한 선택 사항을 지정
	//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		(1) : 전체 화면 모드에서 전면 버퍼의 내용을 화면으로 옮길 때 자동 회전하지 않음
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	(2) : 응용 프로그램이 디스플레이 모드를 변경할 수 있음
	//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		(4) : 응용 프로그램이 GDI 로 랜더링 할 수 있음. 후면 버퍼에 GetDC() 사용 가능
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;

	// SwapEffect : Swaping을 처리하는 선택사항을 지정(기본 : 0)
	//	DXGI_SWAP_EFFECT_DISCARD		(0) : 버퍼 내용을 폐기
	//	DXGI_SWAP_EFFECT_SEQUENTIAL		(1) : 순차 복사
	// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	(2) : Flip 순차 복사
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	
	#pragma endregion

	return CreateSwapChain(dxgiSwapChainDesc);
}

ComPtr<IDXGISwapChain> CDirect3DIndependentDeviceResource::CreateSwapChain(DXGI_SWAP_CHAIN_DESC desc) const
{
	ComPtr<IDXGISwapChain> pdxgiSwapChain;
	if (FAILED(m_dxgiFactory->CreateSwapChain(m_dxgiDevice.Get(), &desc, &pdxgiSwapChain)))
	{
		MessageBox(nullptr, TEXT("SwapChain 인스턴스 생성이 실패했습니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return ComPtr<IDXGISwapChain>();
	}

	return pdxgiSwapChain;
}

ComPtr<ID3D11RenderTargetView> CDirect3DIndependentDeviceResource::CreateRenderTargetViewBySwapChain(IDXGISwapChain * pdxgiSwapChain) const
{
	// 반환되는 값입니다.
	ComPtr<ID3D11RenderTargetView> rtv{ nullptr };

	// Result Handle 입니다. 장치가 성공적으로 생성되는지 검사합니다.
	HRESULT hResult; 
		
	// render target 을 생성하기 위한 back buffer 를 SwapChain 에게 요청합니다.
	ComPtr<ID3D11Texture2D> pd3dBackBuffer{ nullptr };

	hResult = pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<LPVOID*>(&pd3dBackBuffer));
	if (FAILED(hResult)) return rtv;

	// 반환받은 버퍼를 사용하여 render target view 를 생성합니다.
	m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer.Get(), nullptr, &rtv);

	return rtv;
}

ComPtr<ID3D11Texture2D> CDirect3DIndependentDeviceResource::CreateDepthStencilBuffer(RECT rcClient) const
{
	#pragma region [Create Depth-Stencil Buffer]

	// depth stencil "texture" 를 생성합니다.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	
	// 메모리는 0으로 초기화합니다.
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Width : texture 의 너비입니다.
	d3dDepthStencilBufferDesc.Width = rcClient.right;
	// Height : texture 의 높이입니다.
	d3dDepthStencilBufferDesc.Height = rcClient.bottom;
	// MipLevels : texture 최대 MipMap Level 수. 
	//				다중 샘플링 텍스처 : 1
	//				최대 밉맵 레벨 : 0
	d3dDepthStencilBufferDesc.MipLevels = 1;
	// ArraySize :texture 배열의 texture 개수. (배열이 아니면 1)
	d3dDepthStencilBufferDesc.ArraySize = 1;
	// Format : texture 픽셀 형식
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// SampleDesc : 다중 샘플링의 품질을 지정
	// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
	{
		// Count : 픽셀 당 샘플 개수
		//	1  : 다중 샘플링을 하지 않음
		//	2~ : 해당 수만큼의 다중 샘플링
		d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
		// Quality : 품질 레벨
		// 0 : 다중 샘플링을 하지 않음
		d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	}

	// Usage : texture 를 읽고 쓰는 방법에 대한 정의
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// BindFlags : 파이프라인 단계 어디에 연결할 것인지 정의
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	// MiscFlags : 리소스에 대한 추가 선택 사항. 사용하지 않으면 0.
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	// CPUAccessFlags : CPU가 버퍼를 사용할 수 있는 유형. 사용하지 않으면 0.
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;

	#pragma endregion

	return CreateDepthStencilBuffer(d3dDepthStencilBufferDesc);
}

ComPtr<ID3D11Texture2D> CDirect3DIndependentDeviceResource::CreateDepthStencilBuffer(D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc) const
{
	// 반환할 값입니다.
	ComPtr<ID3D11Texture2D> txt2D;

	m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, nullptr, &txt2D);
	
	return txt2D;
}

ComPtr<ID3D11DepthStencilView> CDirect3DIndependentDeviceResource::CreateDepthStencilView(ID3D11Texture2D * pd3dDepthStencilBuffer) const
{
	// 반환할 값입니다.
	ComPtr<ID3D11DepthStencilView> dsv;

	// Depth-Stencil Buffer의 Description을 획득합니다.
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

	// Direct2D 리소스를 초기화합니다.
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)

	// 프로젝트가 디버그 빌드 중인 경우 SDK 레이어를 통해 Direct2D 디버깅을 사용합니다.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

#endif

	// Direct2D 팩터리를 초기화합니다.
	if(FAILED(hResult = D2D1CreateFactory(	  D2D1_FACTORY_TYPE_MULTI_THREADED
											, __uuidof(decltype(m_pd2dFactory)::InterfaceType)
											, &options
											, &m_pd2dFactory
	))) goto ReleaseDXGI;

	// DirectWrite 팩터리를 초기화합니다.
	if (FAILED(hResult = DWriteCreateFactory(	  DWRITE_FACTORY_TYPE_SHARED
												, __uuidof(decltype(m_pdwFactory)::InterfaceType)
												, &m_pdwFactory
	))) goto ReleaseDXGI;
	
	// COM Library를 초기화합니다.
	// UWP에는 없어도 잘 돌아가더니 여기선 안 돌아가네.
	if (FAILED(hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) goto ReleaseDXGI;
	m_bInitializeCoInit = true;

	// WIC(Windows Imaging Container) 팩터리를 초기화합니다.
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
	
	// Direct3D와 Direct2D의 호환을 위한 함수 제공
	m_pd2dFactory->CreateDevice(m_dxgiDevice.Get(), &m_pd2dDevice);

	// Direct2D DC를 생성합니다.
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
	// 화면의 해상도를 얻습니다.
	float fdpiX, fdpiY;
	m_pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
	
	// dxgi format을 알기 위해 Swap Chain 객체의 Description을 획득합니다. 
	DXGI_SWAP_CHAIN_DESC desc;
	pdxgiSwapChain->GetDesc(&desc);
	
	HRESULT hResult;

	// 스왑 체인 백 버퍼에 연결된 Direct2D 대상
	// 비트맵을 만들고 이를 현재 대상으로 설정합니다.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		BitmapProperties1(	  D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW
							, PixelFormat(desc.BufferDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED)
							, fdpiX
							, fdpiY
		);

	// DXGI 표면을 얻습니다.
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
