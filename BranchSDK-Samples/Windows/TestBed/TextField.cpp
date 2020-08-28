#include "TextField.h"

TextField::TextField(LPCWSTR windowName, int x, int y, int width, int height, HWND parent) :
	Window(WS_EX_TRANSPARENT,
		L"Edit",
		windowName,
		WS_VISIBLE | WS_CHILD,
		x,
		y,
		width,
		height,
		parent
	)
{

}