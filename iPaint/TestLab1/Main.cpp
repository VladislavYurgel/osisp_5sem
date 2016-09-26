#include <windows.h>
#include <iostream>
#include "Painter.h"
#include "resource.h"

#define SelectPen(hdc, hpen) \
  ((HPEN)SelectObject((hdc), (HGDIOBJ)(HPEN)(hpen)))

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int currentId = 0;
bool flag = FALSE, flagPoly = FALSE, firstLine = TRUE;
bool flagMouseClick = false;
double zoom = 1;
Painter painter;
POINTS startPointPoly;
char text[2] = { ' ', '\0' };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));

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

	HWND hWindow = CreateWindowEx(NULL,
		L"iPaint", 
		L"iPaint", 
		WS_OVERLAPPEDWINDOW,
		300, 
		200, 
		640, 
		480, 
		NULL,
		NULL,
		hInstance, 
		NULL);

	if (!hWindow)
	{
		int nResult = GetLastError();
		MessageBox(NULL, L"The window has not benn created!", L"Error", MB_ICONERROR);
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
				case ID_FILE_OPEN:
				{

					break;
				}
				case ID_FILE_EXIT:
				{
					hdc = GetDC(hwnd);
					
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
					HDC hdcMeta = CreateEnhMetaFile(hdc, L"D:\\testEMF.emf", &rect, L"Example metafile\0");
					if (!hdcMeta)
					{
						MessageBox(NULL, L"CreateEnhMetaFile!", L"Error", MB_ICONERROR);
					}
					StretchBlt(hdcMeta, 0, 0, GetDeviceCaps(hdc, HORZRES),
						GetDeviceCaps(hdc, VERTRES), memDC, 0, 0,
						GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
					SetMapMode(hdcMeta, MM_ANISOTROPIC);
					ReleaseDC(hwnd, hdc);
					HENHMETAFILE meta = CloseEnhMetaFile(hdcMeta);
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
				case ID_INSTRUMENT_PENCIL:
				{
					if (wParam & MK_LBUTTON)
					{
						hdc = GetDC(hwnd);
						ptsEnd = MAKEPOINTS(lParam);
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

			if (currentId < ID_INSTRUMENT_RECTANGLE && currentId != 0)
			{
				ptsEnd = MAKEPOINTS(lParam);
				MoveToEx(memDC, ptsBegin.x / zoom, ptsBegin.y / zoom, (LPPOINT)NULL);
				LineTo(memDC, ptsEnd.x / zoom, ptsEnd.y / zoom);
			}
			else if (currentId == ID_INSTRUMENT_RECTANGLE)
			{
				SelectObject(memDC, GetStockObject(NULL_BRUSH));
				ptsEnd = MAKEPOINTS(lParam);
				Rectangle(memDC, ptsBegin.x/zoom, ptsBegin.y / zoom, ptsEnd.x / zoom, ptsEnd.y / zoom);
			}
			else if (currentId == ID_INSTRUMENT_ELLIPSE)
			{
				SelectObject(memDC, GetStockObject(NULL_BRUSH));
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
					zoom -= 0.05;
					SendMessage(hwnd, WM_PAINT, 0, 0);
				}
				else
				{
					zoom += 0.05;
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
			break;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}