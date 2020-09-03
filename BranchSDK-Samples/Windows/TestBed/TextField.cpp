#include "TextField.h"

#include <vector>

#include "Windowsx.h"
#include "Winuser.h"

using namespace std;

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

void
TextField::setText(const std::wstring& text)
{
	Edit_SetText(*this, text.c_str());
}

std::wstring
TextField::getText() const
{
	size_t length = Edit_GetTextLength(*this);
	// ensure null termination in the buffer
	vector<wchar_t> buffer(length + 1);
	memset(&buffer[0], 0, length);

	Edit_GetText(*this, &buffer[0], length);
	return wstring(buffer.begin(), buffer.end());
}