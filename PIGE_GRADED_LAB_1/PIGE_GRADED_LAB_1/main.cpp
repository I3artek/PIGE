#include "framework.h"
//#include "tutorial.h"
#include <string>
#include <windowsx.h>

#define MAX_LOADSTRING 100

#define PADDLE_H 80
#define PADDLE_W 40
#define BALL_D 20

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

WCHAR paddleWindowClass[MAX_LOADSTRING];
WCHAR ballWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


ATOM MyRegisterPaddleClass(HINSTANCE hInstance);
BOOL CreatePaddle(HINSTANCE hInstance, int nCmdShow, HWND Parent);
ATOM MyRegisterBallClass(HINSTANCE hInstance);
BOOL CreateBall(HINSTANCE hInstance, int nCmdShow, HWND Parent);


HWND paddle_hwnd;
HWND ball_hwnd;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::wstring title(L" PONG \0");
	title.copy(szTitle, title.size());

	std::wstring className(L" PONG \0");
	className.copy(szWindowClass, className.size());

	std::wstring classPaddleName(L" PADDLE \0");
	classPaddleName.copy(paddleWindowClass, className.size());

	std::wstring classBallName(L" BALL \0");
	classBallName.copy(ballWindowClass, className.size());

	MyRegisterClass(hInstance);
	MyRegisterPaddleClass(hInstance);
	MyRegisterBallClass(hInstance);


	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, nullptr);

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast <int>(msg.wParam);
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = CreateSolidBrush(RGB(140, 255, 140));//reinterpret_cast <HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;

	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}


ATOM MyRegisterPaddleClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;

	//according to https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsyscolor
	//should work, but is not supported in Windows 10
	wcex.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));

	wcex.lpszMenuName = nullptr;

	wcex.lpszClassName = paddleWindowClass;
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}


ATOM MyRegisterBallClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
	wcex.lpszMenuName = nullptr;

	wcex.lpszClassName = ballWindowClass;
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}


BOOL CreatePaddle(HINSTANCE hInstance, int nCmdShow, HWND Parent)
{
	paddle_hwnd = CreateWindowW(paddleWindowClass, szTitle,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CHILD,
		(500 - PADDLE_W), (350 - PADDLE_H) / 2,
		PADDLE_W, PADDLE_H, Parent, nullptr, hInstance,
		nullptr);

	//paddle_hwnd = hWnd;

	if (!paddle_hwnd)
	{
		return FALSE;
	}

	ShowWindow(paddle_hwnd, nCmdShow);
	UpdateWindow(paddle_hwnd);

	return TRUE;
}

BOOL CreateBall(HINSTANCE hInstance, int nCmdShow, HWND Parent)
{
	HWND hWnd = CreateWindowW(ballWindowClass, szTitle,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CHILD,
		(500 - BALL_D) / 2, (350 - BALL_D) / 2,
		BALL_D, BALL_D, Parent, nullptr, hInstance,
		nullptr);

	ball_hwnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	HRGN region = CreateEllipticRgn(0, 0, BALL_D, BALL_D);
	SetWindowRgn(hWnd, region, true);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		(GetSystemMetrics(SM_CXSCREEN) - 500) / 2, (GetSystemMetrics(SM_CYSCREEN) - 350) / 2,
		500, 350, nullptr, nullptr, hInstance,
		nullptr);

	//The following code is from tutorial:
	// 
	// Set WS_EX_LAYERED on this window
	SetWindowLong(hWnd, GWL_EXSTYLE,
		GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// Make this window 50% alpha
	SetLayeredWindowAttributes(hWnd, 0, (255 * 80) / 100, LWA_ALPHA);
	// Show this window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//

		/*
	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 500, 350, nullptr, nullptr, hInstance,
		nullptr);
		*/

	if (!hWnd)
	{
		return FALSE;
	}

	if (!CreatePaddle(hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	if (!CreateBall(hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
			/*
		case IDM_ABOUT:
			// DialogBox (hInst , MAKEINTRESOURCE ( IDD_ABOUTBOX ), hWnd, About );
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
			*/
		case WM_MOUSEMOVE:
			//SetWindowPos(paddle_hwnd, paddle_hwnd, (350 - PADDLE_W), GET_X_LPARAM(lParam) /* + PADDLE_H / 2*/, PADDLE_W, PADDLE_H, SWP_SHOWWINDOW | SWP_NOSIZE);
			MoveWindow(paddle_hwnd, (350 - PADDLE_W), GET_X_LPARAM(lParam) /* + PADDLE_H / 2*/, PADDLE_W, PADDLE_H, TRUE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}

	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return static_cast <INT_PTR>(TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)

		{
			EndDialog(hDlg, LOWORD(wParam));
			return static_cast <INT_PTR>(TRUE);
		}
		break;
	}
	return static_cast <INT_PTR>(FALSE);
}




