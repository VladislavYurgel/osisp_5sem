#pragma once
#include <Windows.h>

#define ZOOM_RATE 0.1
#define ZOOM_MAX_RATE 1.2
#define ZOOM_MIN_RATE 0.8

class Painter
{
public:
	DWORD CurrentColor;
	DWORD FillColor;

	Painter();
	~Painter();

	/* SCROLLBARS */

	void ShowScrollBars(HWND hWnd);
	void HideScrollBars(HWND hWnd);
	void ScrollCheck(HWND, double, double);
	void ScrollBarSetParams(HWND, double);

	/* COLORS */

	void SetPenColor(DWORD);
	void SetFillColor(DWORD);
	DWORD GetPenColor();

	/* INSTRUMENTS */
	
	void drawPencil(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS ptsEnd, double zoom);
	void drawLine(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam);
	void drawRectangle(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam);
	void drawEllipse(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam);
	void drawCurve(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, double zoom, bool fPrevLine, LPARAM lParam);
	void drawPoly(HDC hdc, HDC memDC, POINTS ptsBegin, POINTS *ptsEnd, POINTS startPointPoly, double zoom, bool fPrevLine, LPARAM lParam);
};

