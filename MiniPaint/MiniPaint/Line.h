#pragma once
#include "Shape.h"

class Line : public Shape
{
	public:
		void Draw()
		{
			RECT rt;
			MoveToEx(hDc, pointFirst.x, pointFirst.y, NULL);
			LineTo(hDc, pointSecond.x, pointSecond.y);
			GetClientRect(hWnd, &rt);
		}
};