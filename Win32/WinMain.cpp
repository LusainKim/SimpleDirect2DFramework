// Win32.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Win32.h"
#include "Framework/IndRes/IndependentDeviceResources.h"
#include "Framework/Framework.h"
#include "Framework/Timer/Timer.h"
#include "Scene/Title/TitleScene.h"

#define MAX_LOADSTRING 100

namespace 
{
	int g_iMarginWidth  = 0;
	int g_iMarginHeight = 0;

	CDirectXFramework fwMain;

	shared_ptr<CTimer> timer;
	shared_ptr<CDirect2DIndependentDeviceResource> independentRes;

};

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

int GetMarginWidth() { return g_iMarginWidth; }
int GetMarginHeight() { return g_iMarginHeight; }
HINSTANCE GetHInstance() { return hInst; }

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	timer = make_shared<CTimer>();
	independentRes = make_shared<CDirect2DIndependentDeviceResource>();
	if (!(independentRes->Initialize())) return FALSE;

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.

    while (true)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (!timer->Update()) continue;
			fwMain.FrameAdvance();
		}

    }

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = CDirectXFramework::WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	//	������ ��Ÿ��
	DWORD dwStyle = 0
		| WS_OVERLAPPED 	// ����Ʈ ������. Ÿ��Ʋ �ٿ� ũ�� ������ �ȵǴ� ��輱�� ������. �ƹ��� ��Ÿ�ϵ� ���� ������ �� ��Ÿ���� ����ȴ�.
		| WS_CAPTION 		// Ÿ��Ʋ �ٸ� ���� �����츦 ����� WS_BORDER ��Ÿ���� �����Ѵ�.
		| WS_SYSMENU		// �ý��� �޴��� ���� �����츦 �����.
		| WS_MINIMIZEBOX	// �ּ�ȭ ��ư�� �����.
		| WS_BORDER			// �ܼ����� �� ��輱�� ����� ũ�� ������ �� �� ����.
		| WS_THICKFRAME		// ũ�� ������ ������ �β��� ��輱�� ������. WS_BORDER�� ���� ����� �� ����.
		;					// �߰��� �ʿ��� ������ ��Ÿ���� http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm ����.

	//	�ν��Ͻ� �ڵ��� ���� ������ ����
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	//	����ũ�� ������ ������
	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	//	Ŭ���̾�Ʈ ������
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	//	������ ����� ������ �߰��Ǵ�(ĸ��, �ܰ��� ��) ũ�⸦ ����.
	AdjustWindowRect(&rc, dwStyle, FALSE);

	g_iMarginWidth = rc.right - rc.left - CLIENT_WIDTH;
	g_iMarginHeight = rc.bottom - rc.top - CLIENT_HEIGHT;

	//	Ŭ���̾�Ʈ ������ǥ(left, top)
	//	����ũ���� �߾ӿ� Ŭ���̾�Ʈ�� ��ġ�ϵ��� ����
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	������ ����
	HWND hWnd = CreateWindow(
		  szWindowClass			//	������ Ŭ���� ��
		, szTitle				//	ĸ�� ǥ�� ���ڿ�
		, dwStyle				//	������ ��Ÿ��
		, ptClientWorld.x		//	�θ� ������ ��� ������ ������ǥ : x
		, ptClientWorld.y		//	�θ� ������ ��� ������ ������ǥ : y
		, rc.right - rc.left	//	������ ������ : width
		, rc.bottom - rc.top	//	������ ������ : height
		, NULL					//	�θ� ������.
		, NULL					//	�޴� �ڵ�
		, hInstance				//	�ν��Ͻ� �ڵ�
		, NULL					//	�߰� �Ķ���� : NULL
		);

	//	���� ���н� ���α׷� ����
	//	Ȯ�� : WndProc�� default msg handler�� DefWindowProc �Լ��� ��ȯ�ϴ°�?
	if (!hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage(	  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
						, NULL
						, GetLastError()
						, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
						, reinterpret_cast<LPTSTR>(&lpMsgBuf)
						, 0
						, NULL
		);
		MessageBox(NULL, reinterpret_cast<LPCTSTR>(lpMsgBuf), L"Create Window Fail", MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

	timer->SetUpdateCaptionHwnd(hWnd);

	fwMain.EquipTimer(timer);
	fwMain.InjectIndependentResources(independentRes);

	if (!fwMain.Initialize(hInst, hWnd)) return FALSE;
	fwMain.SetMain();
	fwMain.BuildScene([&] (CDirectXFramework *framework) -> unique_ptr<CScene>
	{
		auto pNewScene { make_unique<CTitleScene>() };

		pNewScene->BuildObjects(TEXT("Title"), framework->GetHwnd(), framework);
		pNewScene->BuildObjecsFromD2D1Devices(independentRes->d2dFactory(), framework->GetRenderTarget(), independentRes->dwFactory(), independentRes->wicFactory());

		return pNewScene;
	});

	fwMain.ChangeScene(TEXT("Title"));

	//	������ ǥ��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//	���� ��ȯ
	return TRUE;
}
