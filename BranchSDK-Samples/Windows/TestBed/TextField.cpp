#include "TextField.h"

#include <vector>

#include "Windowsx.h"
#include "Winuser.h"

using namespace std;

TextField::TextField(LPCWSTR windowName, int x, int y, int width, int height, HMENU menu, HWND parent) :
	Window(WS_EX_TRANSPARENT,
		L"Edit",
		windowName,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
		ES_MULTILINE | ES_AUTOVSCROLL,
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
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
		ES_MULTILINE | ES_AUTOVSCROLL,
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
	// include space for null terminator
	vector<wchar_t> buffer(length + 1);
	memset(&buffer[0], 0, length + 1);

	Edit_GetText(*this, &buffer[0], length + 1);
	return wstring(&buffer[0], &buffer[length]);
}

void
TextField::appendText(const std::wstring& text)
{
	setText(getText() + L"\r\n" + text);
}