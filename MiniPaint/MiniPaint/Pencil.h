#include "Shape.h"

class Pencil : public Shape
{
	private:
		static BOOL Line(HDC hDc, int x1, int y1, int x2, int y2)
		{
			MoveToEx(hDc, x1, y1, NULL);

			return LineTo(hDc, x2, y2);
		}
	public:
		void Draw()
		{
			Line(hDc, pointFirst.x, pointFirst.y, pointSecond.x, pointSecond.y);
		}
};