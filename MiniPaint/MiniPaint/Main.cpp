//#include "Pencil.h"
//#include "Line.h"
#include "Shape.h"
#include "resource.h"

#define coordWindowX 300
#define coordWindowY 200
#define width 640
#define height 480

HINSTANCE hCurrentInstance;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
static bool isDrawing = false;
BOOL DrawLine(HDC, int, int, int, int);
Shape *currentShape = new Pencil();
BOOL palFlag = FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hCurrentInstance = hInstance;

	// Register class

	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));

	wClass.cbSize			= sizeof(WNDCLASSEX);
	wClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wClass.hInstance		= hInstance;
	wClass.cbClsExtra		= 0;
	wClass.cbWndExtra		= 0;
	wClass.lpfnWndProc		= (WNDPROC)WndProc;
	wClass.lpszClassName	= L"My new window";
	wClass.lpszMenuName		= MAKEINTRESOURCEW(IDR_MENU1);

	if (!RegisterClassEx(&wClass))
	{
		int nResult = GetLastError();
		MessageBox(NULL, L"Window class not instance!", L"Error", MB_ICONERROR);
	}

	HWND hWindow = CreateWindowEx(
		NULL, 
		L"My new window",
		L"Mini paint",
		WS_OVERLAPPEDWINDOW,
		coordWindowX, 
		coordWindowY, 
		width, 
		height,
		NULL,
		NULL,
		hInstance, 
		NULL);

	if (!hWindow)
	{
		int nResult = GetLastError();
		MessageBox(NULL, L"The window has not been created", L"Error", MB_ICONERROR);
	}

	ShowWindow(hWindow, nShowCmd);
	UpdateWindow(hWindow);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	currentShape->lParam = lParam;
	currentShape->wParam = wParam;
	currentShape->hWnd = hWnd;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		currentShape->WndProc();
		break;
	}
	case WM_PAINT:
	{
		currentShape->WmPaint();
		break;
	}
	case WM_LBUTTONDOWN:
	{
		currentShape->isDrawing = TRUE;
		currentShape->MouseDown();
		break;
	}
	case WM_MOUSEMOVE:
	{
		currentShape->MouseMove();
		break;
	}
	case WM_LBUTTONUP:
	{
		currentShape->isDrawing = FALSE;
		currentShape->MouseUp();
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_SHAPES_LINE:
		{
			currentShape = new Line();
			currentShape->hWnd = hWnd;
			currentShape->WndProc();
			break;
		}
		case ID_SHAPES_DRAW:
		{
			currentShape = new Pencil();
			currentShape->hWnd = hWnd;
			currentShape->WndProc();
			break;
		}
		case ID_HELP_ABOUT:
		{
			DialogBox(hCurrentInstance, MAKEINTRESOURCE(IDD_DIALOGABOUT), hWnd, About);
			break;
		}
		case ID_FILE_QUIT:
		{
			DestroyWindow(hWnd);
			break;
		}
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (LOWORD(wParam) == MK_CONTROL)
		{
			if ((short)HIWORD(wParam) > 0)
				MessageBox(hWnd, L"Mouse wheel+ with CTRL", L"Mouse wheel", NULL);
			else
				MessageBox(hWnd, L"Mouse wheel- with CTRL", L"Mouse wheel", NULL);
		}
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 'X':
		{
			if (GetAsyncKeyState(VK_CONTROL)) {
				DestroyWindow(hWnd);
				break;
			}
			break;
		}
		}
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return NULL;
}


// About program dialog handler

INT_PTR CALLBACK About(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			default:
			{
				EndDialog(hWnd, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		}
		break;
	}

	return (INT_PTR)FALSE;
}

BOOL DrawLine(HDC hDc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hDc, x1, y1, NULL);

	return LineTo(hDc, x2, y2);
}