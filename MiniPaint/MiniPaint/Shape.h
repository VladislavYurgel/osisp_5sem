#pragma onc
#include <Windows.h>

class Shape
{
	public:
		HDC hDc;
		HWND hWnd;
		HDC tempDc;
		HBITMAP hBitMap;
		BOOL isDrawing = FALSE;
		LPARAM lParam;
		WPARAM wParam;

		BOOL DrawLine(HDC hDc, int x1, int y1, int x2, int y2)
		{
			MoveToEx(hDc, x1, y1, (LPPOINT)NULL);

			return LineTo(hDc, x2, y2);
		}

		virtual void WndProc() {};
		virtual void WmPaint() {};
		virtual void MouseUp() {};
		virtual void MouseMove() {};
		virtual void MouseDown() {};
		virtual void Draw() {};
};

class Line : public Shape
{
	private:
		PAINTSTRUCT ps;
		BOOL prevLine;
		RECT rect;
		HDC dcDraftCopy;
		HDC dcCleanCopy;
		HBITMAP bmDraftCopy;
		HBITMAP bmCleanCopy;
		POINTS ptsBegin;
		POINTS ptsEnd;
		int Ox, Oy;

	public:
		void WndProc()
		{
			hDc = GetDC(hWnd);
			Ox = GetDeviceCaps(hDc, HORZRES);
			Oy = GetDeviceCaps(hDc, VERTRES);
			
			dcCleanCopy = CreateCompatibleDC(hDc);
			bmCleanCopy = CreateCompatibleBitmap(dcCleanCopy, Ox, Oy);
			SelectObject(dcCleanCopy, bmCleanCopy);
			PatBlt(dcCleanCopy, 0, 0, Ox, Oy, PATCOPY);

			dcDraftCopy = CreateCompatibleDC(hDc);
			bmDraftCopy = CreateCompatibleBitmap(dcDraftCopy, Ox, Oy);
			SelectObject(dcDraftCopy, bmDraftCopy);
			PatBlt(dcDraftCopy, 0, 0, Ox, Oy, PATCOPY);

			ReleaseDC(hWnd, hDc);
			DeleteObject(bmCleanCopy);
			DeleteObject(bmDraftCopy);
		}
		void MouseDown()
		{
			ptsBegin = MAKEPOINTS(lParam);
		}
		void MouseMove()
		{
			if (isDrawing)
			{
				ptsEnd = MAKEPOINTS(lParam);
				StretchBlt(dcCleanCopy, 0, 0, Ox, Oy, dcDraftCopy, 0, 0, Ox, Oy, SRCCOPY);
				DrawLine(dcCleanCopy, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
				WmPaint();
			}
		}
		void MouseUp()
		{
			StretchBlt(dcCleanCopy, 0, 0, Ox, Oy, dcDraftCopy, 0, 0, Ox, Oy, SRCCOPY);
			DrawLine(dcCleanCopy, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
			WmPaint();
		}
		void WmPaint()
		{
			hDc = BeginPaint(hWnd, &ps);
			StretchBlt(hDc, 0, 0, Ox, Oy, dcCleanCopy, 0, 0, Ox, Oy, SRCCOPY);
			EndPaint(hWnd, &ps);
		}
		void Draw()
		{

		}
};

class Pencil : public Shape
{
	private:
		PAINTSTRUCT ps;
		RECT rect;
		HPEN hPen;
		POINT point;
		POINT prevPoint;
		POINTS ptsEnd;
		POINTS ptsBegin;
		BOOL palFlag = FALSE;

	public:
		void WndProc()
		{
			tempDc = CreateCompatibleDC(hDc);	
			hBitMap = CreateCompatibleBitmap(hDc, GetDeviceCaps(hDc, HORZRES), GetDeviceCaps(hDc, VERTRES));
			if (!palFlag)
			{
				SelectObject(tempDc, hBitMap);
				PatBlt(tempDc, 0, 0, GetDeviceCaps(hDc, HORZRES), GetDeviceCaps(hDc, VERTRES), WHITENESS);
				palFlag = TRUE;
			}
		}
		void MouseDown()
		{
			ptsBegin = MAKEPOINTS(lParam);
		}
		void MouseMove()
		{
			if (isDrawing)
			{
				hDc = GetDC(hWnd);
				ptsEnd = MAKEPOINTS(lParam);
				DrawLine(hDc, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
				DrawLine(tempDc, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
				ptsBegin = ptsEnd;
				ReleaseDC(hWnd, hDc);
			}
		}
		void WmPaint()
		{
			hDc = GetDC(hWnd);
			StretchBlt(hDc, 0, 0, GetDeviceCaps(hDc, HORZRES), GetDeviceCaps(hDc, VERTRES),
				tempDc, 0, 0, GetDeviceCaps(tempDc, HORZRES), GetDeviceCaps(tempDc, VERTRES), SRCCOPY);
			ReleaseDC(hWnd, hDc);
		}
		void Draw()
		{
			DrawLine(hDc, point.x, point.y, prevPoint.x, prevPoint.y);
			DrawLine(tempDc, point.x, point.y, prevPoint.x, prevPoint.y);
		}
};