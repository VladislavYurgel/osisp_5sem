#pragma onc
#include <Windows.h>

class Shape
{
	public:
		HDC hDc;
		HWND hWnd;
		BOOL isDrawing = FALSE;
		LPARAM lParam;
		WPARAM wParam;
		bool isExecute = FALSE;

		HDC dcDraftCopy;
		HDC dcCleanCopy;
		HBITMAP bmDraftCopy;
		HBITMAP bmCleanCopy;
		POINTS ptsBegin;
		POINTS ptsEnd;
		int Ox, Oy;

		BOOL DrawLine(HDC hDc, int x1, int y1, int x2, int y2)
		{
			MoveToEx(hDc, x1, y1, (LPPOINT)NULL);

			return LineTo(hDc, x2, y2);
		}

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
		};
		virtual void WmPaint() 
		{
			hDc = GetDC(hWnd);
			StretchBlt(hDc, 0, 0, Ox, Oy, dcCleanCopy, 0, 0, Ox, Oy, SRCCOPY);
			ReleaseDC(hWnd, hDc);
		};
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

	public:
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
				StretchBlt(dcDraftCopy, 0, 0, Ox, Oy, dcCleanCopy, 0, 0, Ox, Oy, SRCCOPY);
			}
		}
		void MouseUp()
		{
			StretchBlt(dcCleanCopy, 0, 0, Ox, Oy, dcDraftCopy, 0, 0, Ox, Oy, SRCCOPY);
			DrawLine(dcCleanCopy, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
			StretchBlt(dcDraftCopy, 0, 0, Ox, Oy, dcCleanCopy, 0, 0, Ox, Oy, SRCCOPY);
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
		BOOL palFlag = FALSE;

	public:
		void MouseDown()
		{
			ptsBegin = MAKEPOINTS(lParam);
		}
		void MouseMove()
		{
			if (isDrawing)
			{
				ptsEnd = MAKEPOINTS(lParam);
				DrawLine(dcCleanCopy, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
				ptsBegin = ptsEnd;
			}
		}
};