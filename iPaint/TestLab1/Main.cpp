#include <windows.h>
#include <iostream>
#include "Painter.h"
#include "resource.h"

#define COORDINATE_X 0
#define COORDINATE_Y 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define SelectPen(hdc, hpen) \
  ((HPEN)SelectObject((hdc), (HGDIOBJ)(HPEN)(hpen)))

HINSTANCE hCurrentInstance;

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

bool PrintedArea;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int currentId = 0;
bool flag = FALSE, flagPoly = FALSE, firstLine = TRUE;
bool flagMouseClick = false;
double zoom = 1;
Painter painter;
POINTS startPointPoly;
char text[2] = { ' ', '\0' };
void SaveFileDialog(HDC, HDC, HWND);
void OpenFileDialog(HDC, HDC, HWND);

HWND hWindow;
WNDCLASSEX wClass;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));

	hCurrentInstance = hInstance;

	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW +1);
	wClass.hInstance = hInstance;
	wClass.lpfnWndProc = (WNDPROC)WndProc;
	wClass.lpszClassName = L"iPaint";
	wClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wClass))
	{
		int nResult = GetLastError();
		MessageBox(NULL, L"The window class has not been created!", L"Exit", MB_ICONERROR);
	}

	hWindow = CreateWindowEx(NULL,
		L"iPaint", 
		L"iPaint", 
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		NULL,
		NULL,
		hInstance, 
		NULL);

	painter.HideScrollBars(hWindow);

	if (!hWindow)
	{
		int nResult = GetLastError();
		MessageBox(NULL, L"The window has not benn created!", L"Error", MB_ICONERROR);
	}

	//ShowWindow(hWindow, nShowCmd);
	ShowWindow(hWindow, SW_MAXIMIZE);
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;		
	static POINTS ptsBegin;		
	static POINTS ptsEnd;			
	static BOOL fPrevLine = FALSE;	
	static int k = 0;
	static HDC memDC;
	static HBITMAP memBM;
	static HPEN hPen;

	switch (uMsg)
	{
		case WM_CREATE:
		{
			hdc = GetDC(hwnd);
			k = 0;
			memDC = CreateCompatibleDC(hdc);
			memBM = CreateCompatibleBitmap(hdc, GetDeviceCaps(hdc, HORZRES),
				GetDeviceCaps(hdc, VERTRES));
			if (k == 0)
			{
				SelectObject(memDC, memBM);
				PatBlt(memDC, 0, 0, GetDeviceCaps(hdc, HORZRES), GetDeviceCaps(hdc, VERTRES), WHITENESS);
				k++;
			}
			ReleaseDC(hwnd, hdc);
			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_HELP_ABOUT40014:
				{	
					DialogBox(hCurrentInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, About);
					break;
				}
				case ID_FILE_PRINT:
				{
					currentId = ID_FILE_PRINT;

					if (!PrintedArea)
						break;
					else
						PrintedArea = TRUE;

					OPENFILENAME openFileName;
					char szFileName[MAX_PATH] = "";
					ZeroMemory(&openFileName, sizeof(openFileName));

					openFileName.lStructSize = sizeof(openFileName);
					openFileName.hwndOwner = NULL;
					openFileName.lpstrFilter = (LPCWSTR)L"EMF files (*.emf)\0*.emf";
					openFileName.lpstrFile = (LPWSTR)szFileName;
					openFileName.nMaxFile = MAX_PATH;
					openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					openFileName.lpstrDefExt = (LPCWSTR)L"emf";

					if (GetSaveFileName(&openFileName) == TRUE)
					{
						hdc = GetDC(hwnd);

						RECT rect;
						GetClientRect(hwnd, &rect);
					}

					break;
				}

				case ID_SIZE_VERYSMALL:
				{
					painter.SetPenSize(1);
					break;
				}
				case ID_SIZE_SMALL:
				{
					painter.SetPenSize(3);
					break;
				}
				case ID_SIZE_LARGE:
				{
					painter.SetPenSize(6);
					break;
				}
				case ID_SIZE_VERYLARGE:
				{
					painter.SetPenSize(9);
					break;
				}

				/* Choose color for figure fill */

				case ID_COLOR_FILL:
				{
					CHOOSECOLOR cc;
					static COLORREF acrCustClr[16];
					HBRUSH hbrush;
					static DWORD rgbCurrent;

					ZeroMemory(&cc, sizeof(CHOOSECOLOR));
					
					cc.lStructSize = sizeof(CHOOSECOLOR);
					cc.hwndOwner = hwnd;
					cc.lpCustColors = (LPDWORD)acrCustClr;
					cc.rgbResult = rgbCurrent;
					cc.Flags = CC_FULLOPEN | CC_RGBINIT;
					
					if (ChooseColor(&cc) == TRUE) 
					{
						hbrush = CreateSolidBrush(cc.rgbResult);
						rgbCurrent = cc.rgbResult;
						painter.SetFillColor(rgbCurrent);
					}

					break;
				}
				
				case ID_COLOR_BORDER:
				{
					CHOOSECOLOR cc;
					static COLORREF acrCustClr[16];
					HBRUSH hbrush;
					static DWORD rgbCurrent;

					ZeroMemory(&cc, sizeof(CHOOSECOLOR));

					cc.lStructSize = sizeof(CHOOSECOLOR);
					cc.hwndOwner = hwnd;
					cc.lpCustColors = (LPDWORD)acrCustClr;
					cc.rgbResult = rgbCurrent;
					cc.Flags = CC_FULLOPEN | CC_RGBINIT;

					if (ChooseColor(&cc) == TRUE)
					{
						hbrush = CreateSolidBrush(cc.rgbResult);
						rgbCurrent = cc.rgbResult;
						painter.SetPenColor(rgbCurrent);
					}
					break;
				}

				case ID_FILE_SAVE:
				{
					hdc = GetDC(hwnd);
					SaveFileDialog(hdc, memDC, hwnd);
					ReleaseDC(hwnd, hdc);
					break;
				}

				case ID_FILE_OPEN:
				{
					hdc = GetDC(hwnd);

					OpenFileDialog(hdc, memDC, hwnd);

					break;
				}
				case ID_FILE_EXIT:
				{
					hdc = GetDC(hwnd);
					SaveFileDialog(hdc, memDC, hwnd);
					ReleaseDC(hwnd, hdc);
					break;
				}
				case ID_INSTRUMENT_PENCIL:
				{
					currentId = ID_INSTRUMENT_PENCIL;
					break;
				}
				case ID_INSTRUMENT_LINE:
				{
					currentId = ID_INSTRUMENT_LINE;
					break;
				}
				case ID_INSTRUMENT_RECTANGLE:
				{
					currentId = ID_INSTRUMENT_RECTANGLE;
					break;
				}
				case ID_INSTRUMENT_ELLIPSE:
				{
					currentId = ID_INSTRUMENT_ELLIPSE;
					break;
				}
				case ID_INSTRUMENT_CURVE:
				{
					currentId = ID_INSTRUMENT_CURVE;
					break;
				}
				case ID_INSTRUMENT_OPLY:
				{
					currentId = ID_INSTRUMENT_OPLY;
					firstLine = TRUE;
					break;
				}
				case ID_INSTRUMENT_TEXT:
				{
					currentId = ID_INSTRUMENT_TEXT;
					break;
				}
			}

			break;
		}

		case WM_DESTROY: 
		{
			DeleteObject(memBM);
			PostQuitMessage(0);
			return 0;
			break;
		}

		case WM_LBUTTONDOWN:
		{
			hdc = GetDC(hwnd);
			if ((currentId == ID_INSTRUMENT_CURVE && !flag))
			{
				ptsBegin = MAKEPOINTS(lParam);
				flag = TRUE;
			}
			else if(currentId == ID_INSTRUMENT_OPLY && !flagPoly)
			{
				ptsBegin = MAKEPOINTS(lParam);
				startPointPoly = MAKEPOINTS(lParam);
				flagPoly = TRUE;
			}
			else if (currentId != ID_INSTRUMENT_OPLY && currentId != ID_INSTRUMENT_CURVE)
			{
				ptsBegin = MAKEPOINTS(lParam);
			}
			break;
		}

		case WM_RBUTTONDOWN:
		{
			hdc = GetDC(hwnd);
			if (currentId == ID_INSTRUMENT_CURVE)
			{
				flag = FALSE;
				SendMessage(hwnd, WM_LBUTTONUP, wParam, lParam);
			}
			else if (currentId == ID_INSTRUMENT_OPLY)
			{
				flagPoly = FALSE;
				firstLine = TRUE;
				SendMessage(hwnd, WM_LBUTTONUP, wParam, lParam);
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			switch (currentId)
			{
				case ID_FILE_PRINT:
				{
					if (wParam & MK_LBUTTON)
					{
						hdc = GetDC(hwnd);

						painter.PrintArea(hdc, memDC, ptsBegin, &ptsEnd, zoom, fPrevLine, lParam);
						fPrevLine = TRUE;
						ReleaseDC(hwnd, hdc);
					}

					break;
				}
				case ID_INSTRUMENT_PENCIL:
				{
					if (wParam & MK_LBUTTON)
					{
						hdc = GetDC(hwnd);
						ptsEnd = MAKEPOINTS(lParam);
						
						hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
						SelectObject(hdc, hPen);
						SetDCPenColor(hdc, painter.CurrentColor);
						
						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, painter.FillColor);

						painter.drawPencil(hdc, memDC, ptsBegin, ptsEnd, zoom);
						fPrevLine = TRUE;
						ptsBegin = ptsEnd;
						ReleaseDC(hwnd, hdc);
					}
					break;
				}
				case ID_INSTRUMENT_LINE:
				{
					if (wParam & MK_LBUTTON)
					{
						hdc = GetDC(hwnd);
						
						hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
						SelectObject(hdc, hPen);
						SetDCPenColor(hdc, painter.CurrentColor);
						
						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, painter.FillColor);

						painter.drawLine(hdc, memDC, ptsBegin, &ptsEnd, zoom, fPrevLine, lParam);
						fPrevLine = TRUE;
						ReleaseDC(hwnd, hdc);
					}
					break;
				}
				case ID_INSTRUMENT_RECTANGLE:
				{
					if (wParam & MK_LBUTTON)
					{
						hdc = GetDC(hwnd);
						
						hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
						SelectObject(hdc, hPen);
						SetDCPenColor(hdc, painter.CurrentColor);
						
						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, painter.FillColor);

						painter.drawRectangle(hdc, memDC, ptsBegin, &ptsEnd, zoom, fPrevLine, lParam);
						fPrevLine = TRUE;
						ReleaseDC(hwnd, hdc);
					}
					break;
				}
				case ID_INSTRUMENT_ELLIPSE:
				{ 
					if (wParam & MK_LBUTTON)
					{
						hdc = GetDC(hwnd);
						
						hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
						SelectObject(hdc, hPen);
						SetDCPenColor(hdc, painter.CurrentColor);
						
						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, painter.FillColor);

						painter.drawEllipse(hdc, memDC, ptsBegin, &ptsEnd, zoom, fPrevLine, lParam);
						fPrevLine = TRUE;
						ReleaseDC(hwnd, hdc);
					}
					break;
				}
				case ID_INSTRUMENT_CURVE:
				{
					if (flag)
					{
						hdc = GetDC(hwnd);
						
						hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
						SelectObject(hdc, hPen);
						SetDCPenColor(hdc, painter.CurrentColor);
						
						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, painter.FillColor);

						painter.drawCurve(hdc, memDC, ptsBegin, &ptsEnd, zoom, fPrevLine, lParam);
						fPrevLine = TRUE;
						ReleaseDC(hwnd, hdc);
					}
					break;
				}

				case ID_INSTRUMENT_OPLY:
				{
					if (flagPoly)
					{
						hdc = GetDC(hwnd); 
						
						hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
						SelectObject(hdc, hPen);
						SetDCPenColor(hdc, painter.FillColor);
						
						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, painter.FillColor);

						painter.drawPoly(hdc, memDC, ptsBegin, &ptsEnd, startPointPoly, zoom, fPrevLine, lParam);
						fPrevLine = TRUE;
						ReleaseDC(hwnd, hdc);
					}
					break;
				}
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			hdc = GetDC(hwnd);
			
			if (currentId == ID_FILE_PRINT)
			{
				SendMessage(hwnd, WM_COMMAND, ID_FILE_PRINT, 0);
			}

			hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
			SelectObject(memDC, hPen);
			SetDCPenColor(memDC, painter.CurrentColor);
			
			hPen = CreatePen(PS_SOLID, painter.PenSize, painter.CurrentColor);
			SelectObject(hdc, hPen);
			SetDCPenColor(hdc, painter.CurrentColor);
			
			SelectObject(hdc, GetStockObject(DC_BRUSH));
			SetDCBrushColor(hdc, painter.FillColor);

			SelectObject(memDC, GetStockObject(DC_BRUSH));
			SetDCBrushColor(memDC, painter.FillColor);

			if (currentId < ID_INSTRUMENT_RECTANGLE && currentId != 0)
			{
				ptsEnd = MAKEPOINTS(lParam);
				MoveToEx(memDC, ptsBegin.x / zoom, ptsBegin.y / zoom, (LPPOINT)NULL);
				LineTo(memDC, ptsEnd.x / zoom, ptsEnd.y / zoom);
			}
			else if (currentId == ID_INSTRUMENT_RECTANGLE)
			{
				ptsEnd = MAKEPOINTS(lParam);
				Rectangle(memDC, ptsBegin.x/zoom, ptsBegin.y / zoom, ptsEnd.x / zoom, ptsEnd.y / zoom);
			}
			else if (currentId == ID_INSTRUMENT_ELLIPSE)
			{
				ptsEnd = MAKEPOINTS(lParam);
				Ellipse(memDC, ptsBegin.x / zoom, ptsBegin.y / zoom, ptsEnd.x / zoom, ptsEnd.y / zoom);
			}
			else if (currentId == ID_INSTRUMENT_CURVE)
			{
				fPrevLine = FALSE;
				ptsEnd = MAKEPOINTS(lParam);
				MoveToEx(memDC, ptsBegin.x / zoom, ptsBegin.y / zoom, (LPPOINT)NULL);
				LineTo(memDC, ptsEnd.x / zoom, ptsEnd.y / zoom);
				ptsBegin = ptsEnd;

			}
			else if (currentId == ID_INSTRUMENT_OPLY)
			{
				fPrevLine = FALSE;
				ptsEnd = MAKEPOINTS(lParam);
				if (firstLine)
				{
					MoveToEx(memDC, startPointPoly.x / zoom, startPointPoly.y / zoom, (LPPOINT)NULL);
					LineTo(memDC, ptsEnd.x / zoom, ptsEnd.y / zoom);
					firstLine = FALSE;
				}
				MoveToEx(memDC, ptsBegin.x / zoom, ptsBegin.y / zoom, (LPPOINT)NULL);
				LineTo(memDC, ptsEnd.x / zoom, ptsEnd.y / zoom);
				ptsBegin = ptsEnd;
			}

			fPrevLine = FALSE;
			ClipCursor(NULL);
			ReleaseCapture();

			break;
		}

		case WM_PAINT:
		{
			hdc = GetDC(hwnd);

			if (currentId == ID_INSTRUMENT_TEXT)
			{
				TextOut(memDC, ptsBegin.x, ptsBegin.y, (LPCWSTR)text, sizeof(text));
				ptsBegin.x += 9;
			}
			StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
				GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
				GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
			ReleaseDC(hwnd, hdc);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			if (LOWORD(wParam) == MK_CONTROL)
			{
				if (((short)HIWORD(wParam)) < 0)
				{
					zoom = (zoom - ZOOM_RATE) > ZOOM_MIN_RATE ? zoom - ZOOM_RATE : zoom;
					if ((zoom > (1 - ZOOM_RATE)) && (zoom < (1 + ZOOM_RATE)))
						painter.HideScrollBars(hwnd);
					else
					{
						painter.ScrollBarSetParams(hwnd, zoom);
						painter.ShowScrollBars(hwnd);
					}
					SendMessage(hwnd, WM_PAINT, 0, 0);
				}
				else
				{
					zoom = (zoom - ZOOM_RATE) < ZOOM_MAX_RATE ? zoom + ZOOM_RATE : zoom;
					if ((zoom > (1 - ZOOM_RATE)) && (zoom < (1 + ZOOM_RATE)))
						painter.HideScrollBars(hwnd);
					else
					{
						painter.ShowScrollBars(hwnd);
						painter.ScrollBarSetParams(hwnd, zoom);
					}

					SendMessage(hwnd, WM_PAINT, 0, 0);
				}
			}
			break;
		}

		case WM_KEYDOWN:
		{
			if (currentId == ID_INSTRUMENT_TEXT)
			{
				text[0] = (char)wParam;
				SendMessage(hwnd, WM_PAINT, 0, 0);
			}
			switch (wParam)
			{
				case 'X':
				{
					if (GetAsyncKeyState(VK_CONTROL))
					{
						hdc = GetDC(hwnd);
						SaveFileDialog(hdc, memDC, hwnd);

						DestroyWindow(hwnd);
						break;
					}

					break;
				}
				case 'S':
				{
					if (GetAsyncKeyState(VK_CONTROL))
					{
						hdc = GetDC(hwnd);
						SaveFileDialog(hdc, memDC, hwnd);
						ReleaseDC(hwnd, hdc);
						break;
					}
					break;
				}
				case 'O':
				{
					if (GetAsyncKeyState(VK_CONTROL))
					{
						hdc = GetDC(hwnd);
						OpenFileDialog(hdc, memDC, hwnd);
						ReleaseDC(hwnd, hdc);
						break;
					}
					break;
				}
			}
			break;
		}

		case WM_VSCROLL:
		{

			break;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK About(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	static HBITMAP hBmp;	
	static HWND hPic;
	static HBITMAP hBitmap;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PNG1),
			IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

		return (INT_PTR)TRUE;
	}
	case WM_PAINT:
	{
		HDC hdc;
		RECT rect;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);

		BITMAP bitmap = { 0 };
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hBitmapOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
		StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left,
			rect.bottom - rect.top, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		SelectObject(hMemDC, hBitmapOld);
		DeleteDC(hMemDC);


		EndPaint(hWnd, &ps);

		break;
	}
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

void OpenFileDialog(HDC hdc, HDC memDC, HWND hwnd)
{
	OPENFILENAME openFileName;
	char szFileName[MAX_PATH] = "";
	ZeroMemory(&openFileName, sizeof(openFileName));
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.hwndOwner = NULL;
	openFileName.lpstrFilter = (LPCWSTR)L"EMF files (*.emf)\0*.emf";
	openFileName.lpstrFile = (LPWSTR)szFileName;
	openFileName.nMaxFile = MAX_PATH;
	openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	openFileName.lpstrDefExt = (LPCWSTR)L"emf";

	if (GetOpenFileName(&openFileName) == TRUE)
	{
		HENHMETAFILE henHMetaFile = GetEnhMetaFile(openFileName.lpstrFile);
		RECT rect;
		GetClientRect(hwnd, &rect);
		PlayEnhMetaFile(hdc, henHMetaFile, &rect);

		StretchBlt(memDC, 0, 0, GetDeviceCaps(hdc, HORZRES),
			GetDeviceCaps(hdc, VERTRES), memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);

		DeleteEnhMetaFile(henHMetaFile);
	}
}

void SaveFileDialog(HDC hdc, HDC memDC, HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	int iWidthMM = GetDeviceCaps(hdc, HORZSIZE);
	int iHeightMM = GetDeviceCaps(hdc, VERTSIZE);
	int iWidthPels = GetDeviceCaps(hdc, HORZRES);
	int iHeightPels = GetDeviceCaps(hdc, VERTRES);
	rect.left = (rect.left * iWidthMM * 100) / iWidthPels;
	rect.top = (rect.top * iHeightMM * 100) / iHeightPels;
	rect.right = (rect.right * iWidthMM * 100) / iWidthPels;
	rect.bottom = (rect.bottom * iHeightMM * 100) / iHeightPels;

	/* Create dialog for save file */

	OPENFILENAME openFileName;
	char szFileName[MAX_PATH] = "";
	ZeroMemory(&openFileName, sizeof(openFileName));

	openFileName.lStructSize = sizeof(openFileName);
	openFileName.hwndOwner = NULL;
	openFileName.lpstrFilter = (LPCWSTR)L"EMF files (*.emf)\0*.emf";
	openFileName.lpstrFile = (LPWSTR)szFileName;
	openFileName.nMaxFile = MAX_PATH;
	openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	openFileName.lpstrDefExt = (LPCWSTR)L"emf";

	GetSaveFileName(&openFileName);

	/* Close dialog for save file */

	HDC hdcMeta = CreateEnhMetaFile(hdc, openFileName.lpstrFile, &rect, L"iPaint EMF file\0");
	if (!hdcMeta)
	{
		MessageBox(NULL, L"CreateEnhMetaFile!", L"Error", MB_ICONERROR);
	}
	StretchBlt(hdcMeta, 0, 0, GetDeviceCaps(hdc, HORZRES),
		GetDeviceCaps(hdc, VERTRES), memDC, 0, 0,
		GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
	SetMapMode(hdcMeta, MM_ANISOTROPIC);
	HENHMETAFILE meta = CloseEnhMetaFile(hdcMeta);
}