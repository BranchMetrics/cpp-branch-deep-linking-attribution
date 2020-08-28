#include "Button.h"

Button::Button(LPCWSTR windowName, int x, int y, int width, int height, HWND parent) :
	Window(
		WS_EX_TRANSPARENT,
		L"Button",
		windowName,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x,
		y,
		width,
		height,
		parent
	)
{

}