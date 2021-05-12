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

Button::Button(LPCWSTR text, int x, int y, int width, int height, long long menu, HWND parent) :
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

void
Button::setText(const std::wstring& text)
{
	SendMessage(*this, WM_SETTEXT, 0, (LPARAM)text.c_str());
}