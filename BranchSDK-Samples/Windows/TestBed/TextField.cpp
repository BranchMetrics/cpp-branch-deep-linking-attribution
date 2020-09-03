#include "TextField.h"

#include "Winuser.h"

TextField::TextField(LPCWSTR windowName, int x, int y, int width, int height, HMENU menu, HWND parent) :
	Window(WS_EX_TRANSPARENT,
		L"Edit",
		windowName,
		WS_VISIBLE | WS_CHILD,
		x,
		y,
		width,
		height,
		menu,
		parent
	)
{

}

TextField::TextField(LPCWSTR windowName, int x, int y, int width, int height, int menu, HWND parent) :
	Window(WS_EX_TRANSPARENT,
		L"Edit",
		windowName,
		WS_VISIBLE | WS_CHILD,
		x,
		y,
		width,
		height,
		menu,
		parent
	)
{

}

HWND
TextField::create(HWND parent)
{
	HWND hwnd = Window::create(parent);
	SendMessage(hwnd, EM_SETREADONLY, TRUE, NULL);
	return hwnd;
}