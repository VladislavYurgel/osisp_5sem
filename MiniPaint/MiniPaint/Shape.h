#pragma onc
#include <Windows.h>

class Shape
{
	public:
		BOOL fPrevLine = FALSE;
		HDC hDc;
		HWND hWnd;
		POINT pointFirst;
		POINT pointSecond;

		virtual void WndProc() {};
		virtual void MouseUp() {};
		virtual void MouseMove() {};
		virtual void MouseDown() {};
		virtual void Draw() {};
};

class Line : public Shape
{
	public:
		void MouseDown()
		{

		}
		void Draw()
		{
			MoveToEx(hDc, pointFirst.x, pointFirst.y, NULL);
			LineTo(hDc, pointSecond.x, pointSecond.y);
		}
};

class Pencil : public Shape
{
	private:
		static BOOL DrawLine(HDC hDc, int x1, int y1, int x2, int y2)
		{
			MoveToEx(hDc, x1, y1, NULL);

			return LineTo(hDc, x2, y2);
		}
	public:
		void MouseDown()
		{

		}
		void Draw()
		{
			DrawLine(hDc, pointFirst.x, pointFirst.y, pointSecond.x, pointSecond.y);
		}
};