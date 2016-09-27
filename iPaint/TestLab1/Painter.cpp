#include "Painter.h"


Painter::Painter()
{
}


Painter::~Painter()
{
}

void Painter::SetPenSize(int newSize)
{
	PenSize = newSize;
}

void Painter::SetFillColor(DWORD color)
{
	FillColor = color;
}

void Painter::SetPenColor(DWORD color)
{
	CurrentColor = color;
}

DWORD Painter::GetPenColor()
{
	return CurrentColor;
}

void Painter::ShowScrollBars(HWND hWnd)
{
	ShowScrollBar(hWnd, SB_HORZ, TRUE);
	ShowScrollBar(hWnd, SB_VERT, TRUE);
}

void Painter::ScrollBarSetParams(HWND hWnd, double zoom)
{
	int pageSizeX, pageSizeY;
	int xMaxScroll, yMaxScroll;

	HDC hDc = GetDC(hWnd);

	if (zoom == 1)
	{
		pageSizeX = GetDeviceCaps(hDc, HORZRES);
		pageSizeY = GetDeviceCaps(hDc, VERTRES);
	}
	else
	{
		pageSizeX = GetDeviceCaps(hDc, HORZRES);
		pageSizeY = GetDeviceCaps(hDc, VERTRES);
	}

	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_ALL;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = 1;
	scrollInfo.nPage = 1;
	SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);
	SetScrollInfo(hWnd, SB_HORZ, &scrollInfo, TRUE);
}

void Painter::HideScrollBars(HWND hWnd)
{
	ShowScrollBar(hWnd, SB_HORZ, FALSE);
	ShowScrollBar(hWnd, SB_VERT, FALSE);
}

void Painter::ScrollCheck(HWND hWnd, double width, double height)
{
	HDC hDc = GetDC(hWnd);
	double newWidth = GetDeviceCaps(hDc, HORZRES);
	double newHeight = GetDeviceCaps(hDc, VERTRES);
}

void Painter::PrintArea(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam)
{
	if (fPrevLine)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		SetROP2(hdc, R2_NOTXORPEN); //R2_NOTXORPEN
		Rectangle(hdc, ptsBegin.x, ptsBegin.y, ptsEnd->x, ptsEnd->y);
		StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
			GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
		SetROP2(hdc, R2_COPYPEN);

	}
	*ptsEnd = MAKEPOINTS(lParam);

	Rectangle(hdc, ptsBegin.x, ptsBegin.y, ptsEnd->x, ptsEnd->y);
}

void Painter::drawPencil(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS ptsEnd, double zoom)
{
	MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
	LineTo(hdc, ptsEnd.x, ptsEnd.y);

	MoveToEx(memDC, ptsBegin.x / zoom, ptsBegin.y / zoom, (LPPOINT)NULL);
	LineTo(memDC, ptsEnd.x / zoom, ptsEnd.y / zoom);
}

void Painter::drawLine(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam)
{
	if (fPrevLine)
	{
		SetROP2(hdc, R2_NOTXORPEN); //R2_NOTXORPEN

		MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
		LineTo(hdc, ptsEnd->x, ptsEnd->y);
		StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
			GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
		SetROP2(hdc, R2_COPYPEN);
	}
	*ptsEnd = MAKEPOINTS(lParam);

	MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
	LineTo(hdc, ptsEnd->x, ptsEnd->y);
}

void Painter::drawRectangle(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam)
{
	if (fPrevLine)
	{
		Rectangle(hdc, ptsBegin.x, ptsBegin.y, ptsEnd->x, ptsEnd->y);
		StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
			GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);

	}
	*ptsEnd = MAKEPOINTS(lParam);

	Rectangle(hdc, ptsBegin.x, ptsBegin.y, ptsEnd->x, ptsEnd->y);
}

void Painter::drawEllipse(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam)
{
	if (fPrevLine)
	{
		SetROP2(hdc, R2_NOTXORPEN);

		Ellipse(hdc, ptsBegin.x, ptsBegin.y, ptsEnd->x, ptsEnd->y);
		StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
			GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
		SetROP2(hdc, R2_COPYPEN);

	}
	*ptsEnd = MAKEPOINTS(lParam);

	Ellipse(hdc, ptsBegin.x, ptsBegin.y, ptsEnd->x, ptsEnd->y);
}

void Painter::drawCurve(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam)
{
	if (fPrevLine)
	{
		SetROP2(hdc, R2_NOTXORPEN);
		MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
		LineTo(hdc, ptsEnd->x, ptsEnd->y);
		StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
			GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
		SetROP2(hdc, R2_COPYPEN);
	}
	*ptsEnd = MAKEPOINTS(lParam);

	MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
	LineTo(hdc, ptsEnd->x, ptsEnd->y);
}

void Painter::drawPoly(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, POINTS startPointPoly, double zoom, bool fPrevLine, LPARAM lParam)
{
	if (fPrevLine)
	{
		SetROP2(hdc, R2_NOTXORPEN); //R2_NOTXORPEN
		MoveToEx(hdc, startPointPoly.x, startPointPoly.y, (LPPOINT)NULL);
		LineTo(hdc, ptsEnd->x, ptsEnd->y);
		MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
		LineTo(hdc, ptsEnd->x, ptsEnd->y);
		StretchBlt(hdc, 0, 0, GetDeviceCaps(hdc, HORZRES)*zoom,
			GetDeviceCaps(hdc, VERTRES)*zoom, memDC, 0, 0,
			GetDeviceCaps(memDC, HORZRES), GetDeviceCaps(memDC, VERTRES), SRCCOPY);
		SetROP2(hdc, R2_COPYPEN);
	}
	*ptsEnd = MAKEPOINTS(lParam);

	MoveToEx(hdc, startPointPoly.x, startPointPoly.y, (LPPOINT)NULL);
	LineTo(hdc, ptsEnd->x, ptsEnd->y);
	MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
	LineTo(hdc, ptsEnd->x, ptsEnd->y);
}