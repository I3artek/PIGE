#include "framework.h"
#include "resource.h"
#include <string>
#include <windowsx.h>
#include <Windows.h>
#include <commdlg.h>

/*
* What works:
* Main menu and game reset: 1 point
* Changing background: 2 points
*/

#define MAX_LOADSTRING 100

#define PADDLE_H 80
#define PADDLE_W 30
#define BALL_D 20
#define MAIN_WIN_H 350
#define MAIN_WIN_W 500
#define BALL_MOVE_TIMEOUT 50
#define BALL_SPEED 4

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

WCHAR paddleWindowClass[MAX_LOADSTRING];
WCHAR ballWindowClass[MAX_LOADSTRING];

// https://learn.microsoft.com/pl-pl/windows/win32/dlgbox/using-common-dialog-boxes?redirectedfrom=MSDN
static COLORREF acrCustClr[16];
static DWORD rgbCurrent;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


ATOM MyRegisterPaddleClass(HINSTANCE hInstance);
BOOL CreatePaddle(HINSTANCE hInstance, int nCmdShow, HWND Parent);
ATOM MyRegisterBallClass(HINSTANCE hInstance);
BOOL CreateBall(HINSTANCE hInstance, int nCmdShow, HWND Parent);

VOID CALLBACK MoveBall(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);


void movePaddle(HWND main_win, LPARAM lParam);

void reset_game();

HWND paddle_hwnd;
HWND ball_hwnd;
HWND main_hwnd;

HWND left_counter;
HWND right_counter;

//top left point - global later
POINT paddle_pos = { (MAIN_WIN_W - PADDLE_W), (MAIN_WIN_H - PADDLE_H) / 2 };

//top left point
POINT ball_pos = { (MAIN_WIN_W - BALL_D) / 2, (MAIN_WIN_H - BALL_D) / 2 };
POINT ball_pos_initial = ball_pos;
POINT ball_v = { BALL_SPEED, BALL_SPEED };

HBRUSH bg_color;

BOOL ball_hit_right_edge = FALSE;

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

	MoveBall(ball_hwnd, NULL, 1, BALL_MOVE_TIMEOUT);

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
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

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
		(MAIN_WIN_W - PADDLE_W), (MAIN_WIN_H - PADDLE_H) / 2,
		PADDLE_W, PADDLE_H, Parent, nullptr, hInstance,
		nullptr);

	//paddle_hwnd = hWnd;

	ClientToScreen(main_hwnd, &paddle_pos);

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
		(MAIN_WIN_W - BALL_D) / 2, (MAIN_WIN_H - BALL_D) / 2,
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
	main_hwnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		(GetSystemMetrics(SM_CXSCREEN) - MAIN_WIN_W) / 2, (GetSystemMetrics(SM_CYSCREEN) - MAIN_WIN_H) / 2,
		MAIN_WIN_W, MAIN_WIN_H, nullptr, nullptr, hInstance,
		nullptr);

	//The following code is from tutorial:
	// 
	// Set WS_EX_LAYERED on this window
	SetWindowLong(main_hwnd, GWL_EXSTYLE,
		GetWindowLong(main_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// Make this window 50% alpha
	SetLayeredWindowAttributes(main_hwnd, 0, (255 * 80) / 100, LWA_ALPHA);
	// Show this window
	ShowWindow(main_hwnd, nCmdShow);
	UpdateWindow(main_hwnd);
	//

		/*
	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 500, 350, nullptr, nullptr, hInstance,
		nullptr);
		*/

	if (!main_hwnd)
	{
		return FALSE;
	}

	if (!CreatePaddle(hInstance, nCmdShow, main_hwnd))
	{
		return FALSE;
	}

	if (!CreateBall(hInstance, nCmdShow, main_hwnd))
	{
		return FALSE;
	}

	left_counter = CreateWindow((LPCWSTR)"STATIC", (LPCWSTR)"7", WS_VISIBLE | WS_CHILD | SS_LEFT, MAIN_WIN_W / 4, 100, 100, 100, main_hwnd, NULL, hInstance, NULL);
	right_counter = CreateWindow((LPCWSTR)"STATIC", (LPCWSTR)"3", WS_VISIBLE | WS_CHILD | SS_LEFT, MAIN_WIN_W * 3 / 4, 100, 100, 100, main_hwnd, NULL, hInstance, NULL);

	ShowWindow(main_hwnd, nCmdShow);
	UpdateWindow(main_hwnd);

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
			case ID_FILE_EXIT:
				DestroyWindow(main_hwnd);
				break;
				
			case ID_HELP_ABOUT:
				DialogBox (hInst, MAKEINTRESOURCE(ID_HELP_ABOUT), hWnd, About);
				break;
			case ID_BACKGROUND_COLOR:
				CHOOSECOLOR cc;

				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = main_hwnd;
				cc.rgbResult = rgbCurrent;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;
				cc.lpCustColors = (LPDWORD)acrCustClr;

				if (ChooseColor(&cc) == TRUE)
				{
					bg_color = CreateSolidBrush(cc.rgbResult);
					SetClassLongPtr(main_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)bg_color);
					InvalidateRect(main_hwnd, 0, TRUE);
				}
				break;
			case ID_FILE_NEWGAME:
				reset_game();
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
		case WM_MOUSEMOVE:
			movePaddle(hWnd, lParam);
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




void movePaddle(HWND main_win, LPARAM lParam)
{
	//tried to prevent the paddle from going halfway out of the bounds - did not work :(
	/*
	int y = GET_Y_LPARAM(lParam) - PADDLE_H / 2;
	POINT p = { 0, GET_Y_LPARAM(lParam) };
	ScreenToClient(main_win, &p);
	y = min(MAIN_WIN_H - PADDLE_H / 2, max(0 + PADDLE_H / 2, p.y));
	p.y = y;
	ClientToScreen(main_win, &p);
	SetWindowPos(paddle_hwnd, NULL, MAIN_WIN_W - PADDLE_W, p.y, PADDLE_W, PADDLE_H, SWP_SHOWWINDOW | SWP_NOSIZE);
	*/
	paddle_pos.y = GET_Y_LPARAM(lParam) - PADDLE_H / 2;
	SetWindowPos(paddle_hwnd, NULL, MAIN_WIN_W - PADDLE_W, paddle_pos.y, PADDLE_W, PADDLE_H, SWP_SHOWWINDOW | SWP_NOSIZE);
}

void DetectCollisions()
{
	if (ball_pos.y <= 0)
	{
		ball_v.y *= -1;
		ball_pos.y += ball_v.y * 2;
	}
	if (ball_pos.y >= (MAIN_WIN_H - 4 * BALL_D))
	{
		ball_v.y *= -1;
		ball_pos.y += ball_v.y * 2;
	}
	if (ball_pos.x <= 0)
	{
		ball_v.x *= -1;
		ball_pos.x += ball_v.x * 2;
	}
	if (paddle_pos.y <= ball_pos.y && (paddle_pos.y + PADDLE_H) >= ball_pos.y)
	{
		if (ball_pos.x >= (MAIN_WIN_W - PADDLE_W - BALL_D))
		{
			ball_v.x *= -1;
			ball_pos.x += ball_v.x * 2;
		}
	}
	if (ball_pos.x >= (MAIN_WIN_W - 2 * BALL_D))
	{
		ball_v.x *= -1;
		ball_pos.x += ball_v.x * 2;
		ball_v = { 0,0 };
	}
}

//just the arguments
//https://cboard.cprogramming.com/windows-programming/8923-how-do-you-use-timerproc.html
VOID CALLBACK MoveBall(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	ball_pos.x += ball_v.x;
	ball_pos.y += ball_v.y;
	DetectCollisions();
	SetWindowPos(ball_hwnd, NULL, ball_pos.x, ball_pos.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	if (!ball_hit_right_edge)
	{
		SetTimer(hwnd, idEvent, BALL_MOVE_TIMEOUT, (TIMERPROC)MoveBall);
	}
}


void reset_game()
{
	ball_pos = ball_pos_initial;
	ball_v = { BALL_SPEED, BALL_SPEED };
}


