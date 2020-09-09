#include "Button.h"

Button::Button(LPCWSTR text, int x, int y, int width, int height, HMENU menu, HWND parent) :
	Window(
		WS_EX_TRANSPARENT,
		L"Button",
		text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x,
		y,
		width,
		height,
		menu,
		parent
	)
{

}

Button::Button(LPCWSTR text, int x, int y, int width, int height, int menu, HWND parent) :
	Window(
		WS_EX_TRANSPARENT,
		L"Button",
		text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x,
		y,
		width,
		height,
		menu,
		parent
	)
{

}